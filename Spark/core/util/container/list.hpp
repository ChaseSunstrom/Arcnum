#ifndef SPARK_LIST_HPP
#define SPARK_LIST_HPP

#include <core/util/memory/allocator.hpp>
#include <core/util/types.hpp>
#include <initializer_list>

namespace Spark {

	template<typename _Ty> class ListIterator;

	template<typename _Ty, typename _Allocator = Allocator<_Ty>> class List {
	  private:
		struct Node {
			_Ty   data;
			Node* next;
			Node* prev;
			Node(const _Ty& value)
				: data(value)
				, next(nullptr)
				, prev(nullptr) {}
			Node(_Ty&& value) noexcept
				: data(Move(value))
				, next(nullptr)
				, prev(nullptr) {}
		};

	  public:
		using NodeAllocatorType   = typename AllocatorTraits<_Allocator>::template RebindAlloc<Node>;
		using NodeAllocatorTraits = AllocatorTraits<NodeAllocatorType>;

		using ValueType           = _Ty;
		using Pointer             = typename NodeAllocatorTraits::Pointer;
		using ConstPointer        = typename NodeAllocatorTraits::ConstPointer;
		using Reference           = ValueType&;
		using ConstReference      = const ValueType&;
		using SizeType            = typename NodeAllocatorTraits::SizeType;
		using DifferenceType      = typename NodeAllocatorTraits::DifferenceType;

		using Iterator            = ListIterator<_Ty>;
		using ConstIterator       = ListIterator<_Ty>;

		explicit List(const _Allocator& alloc = _Allocator())
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0)
			, m_allocator(alloc) {}

		List(const List& other)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0)
			, m_allocator(NodeAllocatorTraits::SelectOnContainerCopyConstruction(other.m_allocator)) {
			for (const auto& value : other) {
				PushBack(value);
			}
		}

		List(List&& other) noexcept
			: m_head(other.m_head)
			, m_tail(other.m_tail)
			, m_size(other.m_size)
			, m_allocator(Move(other.m_allocator)) {
			other.m_head = nullptr;
			other.m_tail = nullptr;
			other.m_size = 0;
		}

		List(const std::initializer_list<_Ty>& values, const _Allocator& alloc = _Allocator())
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0)
			, m_allocator(alloc) {
			for (const auto& value : values) {
				PushBack(value);
			}
		}

		~List() { Clear(); }

		List& operator=(const List& other) {
			if (this != &other) {
				Clear();
				if (NodeAllocatorTraits::PropagateOnContainerCopyAssignment) {
					m_allocator = other.m_allocator;
				}
				for (const auto& value : other) {
					PushBack(value);
				}
			}
			return *this;
		}

		List& operator=(List&& other) noexcept {
			if (this != &other) {
				Clear();
				m_head       = other.m_head;
				m_tail       = other.m_tail;
				m_size       = other.m_size;
				if (NodeAllocatorTraits::PropagateOnContainerMoveAssignment) {
					m_allocator  = Move(other.m_allocator);
				}
				other.m_head = nullptr;
				other.m_tail = nullptr;
				other.m_size = 0;
			}
			return *this;
		}

		void PushFront(const _Ty& value) {
			Node* new_node = CreateNode(value);
			new_node->next = m_head;
			if (m_head) {
				m_head->prev = new_node;
			}
			m_head = new_node;
			if (!m_tail) {
				m_tail = m_head;
			}
			++m_size;
		}

		void PushFront(_Ty&& value) {
			Node* new_node = CreateNode(Move(value));
			new_node->next = m_head;
			if (m_head) {
				m_head->prev = new_node;
			}
			m_head = new_node;
			if (!m_tail) {
				m_tail = m_head;
			}
			++m_size;
		}

		void PopFront() {
			if (Empty()) return;
			Node* temp = m_head;
			m_head = m_head->next;
			if (m_head) {
				m_head->prev = nullptr;
			} else {
				m_tail = nullptr;
			}
			DestroyNode(temp);
			--m_size;
		}

		void PushBack(const _Ty& value) {
			Node* new_node = CreateNode(value);
			if (m_tail) {
				m_tail->next = new_node;
				new_node->prev = m_tail;
				m_tail = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			++m_size;
		}

		void PushBack(_Ty&& value) {
			Node* new_node = CreateNode(Move(value));
			if (m_tail) {
				m_tail->next = new_node;
				new_node->prev = m_tail;
				m_tail = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			++m_size;
		}

		void PopBack() {
			if (Empty()) return;
			Node* temp = m_tail;
			m_tail = m_tail->prev;
			if (m_tail) {
				m_tail->next = nullptr;
			} else {
				m_head = nullptr;
			}
			DestroyNode(temp);
			--m_size;
		}

		void Clear() {
			while (m_head) {
				Node* temp = m_head;
				m_head = m_head->next;
				DestroyNode(temp);
			}
			m_tail = nullptr;
			m_size = 0;
		}

		void Swap(List& other) noexcept {
			_SPARK Swap(m_head, other.m_head);
			_SPARK Swap(m_tail, other.m_tail);
			_SPARK Swap(m_size, other.m_size);
			if (NodeAllocatorTraits::PropagateOnContainerSwap) {
				_SPARK Swap(m_allocator, other.m_allocator);
			}
		}

		void Remove(const _Ty& value) {
			for (Node* current = m_head; current != nullptr; ) {
				if (current->data == value) {
					Node* temp = current;
					current = current->next;
					EraseNode(temp);
				} else {
					current = current->next;
				}
			}
		}

		Iterator Erase(Iterator pos) {
			if (pos == End()) return End();
			Node* node = pos.m_node;
			Node* next = node->next;
			EraseNode(node);
			return Iterator(next);
		}

		Iterator Erase(Iterator first, Iterator last) {
			while (first != last) {
				first = Erase(first);
			}
			return Iterator(last.m_node);
		}

		Reference At(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("List index out of range");
			}
			return *_SPARK Next(Begin(), index);
		}

		ConstReference At(SizeType index) const {
			if (index >= m_size) {
				throw LOG_FATAL("List index out of range");
			}
			return *_SPARK Next(Begin(), index);
		}

		Iterator      Begin() { return Iterator(m_head); }
		Iterator      End() { return Iterator(nullptr); }
		ConstIterator Begin() const { return ConstIterator(m_head); }
		ConstIterator End() const { return ConstIterator(nullptr); }
		ConstIterator CBegin() const { return ConstIterator(m_head); }
		ConstIterator CEnd() const { return ConstIterator(nullptr); }

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }
		ConstIterator cbegin() const { return CBegin(); }
		ConstIterator cend() const { return CEnd(); }

		Reference      Front() { return m_head->data; }
		ConstReference Front() const { return m_head->data; }

		Reference      Back() { return m_tail->data; }
		ConstReference Back() const { return m_tail->data; }

		bool     Empty() const { return m_size == 0; }
		SizeType Size() const { return m_size; }

		bool operator==(const List& other) const {
			return Size() == other.Size() && _SPARK Equal(Begin(), End(), other.Begin());
		}

		bool operator!=(const List& other) const { return !(*this == other); }

		NodeAllocatorType GetAllocator() const { return m_allocator; }

	  private:
		Node*             m_head;
		Node*             m_tail;
		SizeType          m_size;
		NodeAllocatorType m_allocator;

		Node* CreateNode(const _Ty& value) {
			Node* node = NodeAllocatorTraits::Allocate(m_allocator, 1);
			try {
				NodeAllocatorTraits::Construct(m_allocator, node, value);
			} catch (...) {
				NodeAllocatorTraits::Deallocate(m_allocator, node, 1);
				throw;
			}
			return node;
		}

		Node* CreateNode(_Ty&& value) {
			Node* node = NodeAllocatorTraits::Allocate(m_allocator, 1);
			try {
				NodeAllocatorTraits::Construct(m_allocator, node, Move(value));
			} catch (...) {
				NodeAllocatorTraits::Deallocate(m_allocator, node, 1);
				throw;
			}
			return node;
		}

		void DestroyNode(Node* node) {
			NodeAllocatorTraits::Destroy(m_allocator, node);
			NodeAllocatorTraits::Deallocate(m_allocator, node, 1);
		}

		void EraseNode(Node* node) {
			if (node->prev) {
				node->prev->next = node->next;
			} else {
				m_head = node->next;
			}
			if (node->next) {
				node->next->prev = node->prev;
			} else {
				m_tail = node->prev;
			}
			DestroyNode(node);
			--m_size;
		}

		friend class ListIterator<_Ty>;
		friend class ListIterator<const _Ty>;
	};

	template<typename _Ty> class ListIterator : public Iterator<ForwardIteratorTag, _Ty> {
	  public:
		using IteratorCategory = ForwardIteratorTag;
		using DifferenceType   = ptrdiff_t;
		using ValueType        = _Ty;
		using Pointer          = ValueType*;
		using Reference        = ValueType&;
		using ConstPointer     = const ValueType*;
		using ConstReference   = const ValueType&;
		using Node             = typename List<_Ty>::Node;

		ListIterator(Node* node)
			: m_node(node) {}

		// Non-const operator*
		Reference operator*() { return m_node->data; }

		// Const operator*
		ConstReference operator*() const { return m_node->data; }

		// Non-const operator->
		Pointer operator->() { return &(m_node->data); }

		// Const operator->
		ConstPointer operator->() const { return &(m_node->data); }

		ListIterator& operator++() {
			m_node = m_node->next;
			return *this;
		}

		ListIterator operator++(i32) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const ListIterator& other) const { return m_node == other.m_node; }
		bool operator!=(const ListIterator& other) const { return m_node != other.m_node; }

	  private:
		Node* m_node;
		friend class List<_Ty>;
	};

	template<typename _Ty, typename Allocator> void Swap(List<_Ty, Allocator>& a, List<_Ty, Allocator>& b) { a.Swap(b); }
} // namespace Spark

#endif // SPARK_LIST_HPP