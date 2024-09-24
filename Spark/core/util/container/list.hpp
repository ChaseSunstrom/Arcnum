#ifndef SPARK_LIST_HPP
#define SPARK_LIST_HPP

#include <core/util/types.hpp>
#include <core/util/memory/allocator.hpp>
#include <initializer_list>

namespace Spark {
	template<typename _Ty, typename Allocator = _SPARK Allocator<_Ty>> class List {
	  private:
		struct Node {
			_Ty   data;
			Node* next;
			Node(const _Ty& value)
				: data(value)
				, next(nullptr) {}
			Node(_Ty&& value)
				: data(Move(value))
				, next(nullptr) {}
		};

		using NodeAllocator = typename Allocator::template rebind<Node>::other;

	  public:
		using Pointer        = _Ty*;
		using ConstPointer   = const _Ty*;
		using Reference      = _Ty&;
		using ConstReference = const _Ty&;

		class Iterator {
		  public:
			Iterator(Node* node)
				: m_node(node) {}

			Reference operator*() const { return m_node->data; }
			Pointer   operator->() { return &(m_node->data); }

			Iterator& operator++() {
				m_node = m_node->next;
				return *this;
			}

			Iterator operator++(int) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const Iterator& other) const { return m_node == other.m_node; }
			bool operator!=(const Iterator& other) const { return m_node != other.m_node; }

		  private:
			Node* m_node;
			friend class List;
		};

		using ConstIterator = Iterator;

		explicit List(const Allocator& alloc = Allocator())
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0)
			, m_allocator(alloc) {}

		List(const List& other)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0)
			, m_allocator(other.m_allocator) {
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

		List(const std::initializer_list<_Ty>& values, const Allocator& alloc = Allocator())
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
				m_allocator = other.m_allocator;
				for (const auto& value : other) {
					PushBack(value);
				}
			}
			return *this;
		}

		List& operator=(List&& other) noexcept {
			if (this != &other) {
				Clear();
				m_head = other.m_head;
				m_tail = other.m_tail;
				m_size = other.m_size;
				m_allocator = Move(other.m_allocator);
				other.m_head = nullptr;
				other.m_tail = nullptr;
				other.m_size = 0;
			}
			return *this;
		}

		void PushFront(const _Ty& value) {
			Node* new_node = m_allocator.allocate(1);
			m_allocator.construct(new_node, value);
			new_node->next = m_head;
			m_head = new_node;
			if (!m_tail) {
				m_tail = m_head;
			}
			m_size++;
		}

		void PushFront(_Ty&& value) {
			Node* new_node = m_allocator.allocate(1);
			m_allocator.construct(new_node, Move(value));
			new_node->next = m_head;
			m_head = new_node;
			if (!m_tail) {
				m_tail = m_head;
			}
			m_size++;
		}

		void PopFront() {
			if (m_size == 0)
				return;
			Node* temp = m_head;
			m_head = m_head->next;
			m_allocator.destroy(temp);
			m_allocator.deallocate(temp, 1);
			m_size--;
			if (m_size == 0) {
				m_tail = nullptr;
			}
		}

		void PushBack(const _Ty& value) {
			Node* new_node = m_allocator.allocate(1);
			m_allocator.construct(new_node, value);
			if (m_tail) {
				m_tail->next = new_node;
				m_tail = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			m_size++;
		}

		void PushBack(_Ty&& value) {
			Node* new_node = m_allocator.allocate(1);
			m_allocator.construct(new_node, Move(value));
			if (m_tail) {
				m_tail->next = new_node;
				m_tail = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			m_size++;
		}

		void PopBack() {
			if (m_size == 0)
				return;
			if (m_size == 1) {
				m_allocator.destroy(m_head);
				m_allocator.deallocate(m_head, 1);
				m_head = m_tail = nullptr;
				m_size = 0;
				return;
			}
			Node* current = m_head;
			while (current->next != m_tail) {
				current = current->next;
			}
			m_allocator.destroy(m_tail);
			m_allocator.deallocate(m_tail, 1);
			m_tail = current;
			m_tail->next = nullptr;
			m_size--;
		}

		void Clear() {
			while (m_head) {
				Node* temp = m_head;
				m_head = m_head->next;
				m_allocator.destroy(temp);
				m_allocator.deallocate(temp, 1);
			}
			m_tail = nullptr;
			m_size = 0;
		}

		void Swap(List& other) {
			_SPARK Swap(m_head, other.m_head);
			_SPARK Swap(m_tail, other.m_tail);
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_allocator, other.m_allocator);
		}

		void Remove(const _Ty& value) {
			Node** current = &m_head;
			while (*current) {
				if ((*current)->data == value) {
					Node* temp = *current;
					*current = (*current)->next;
					if (m_tail == temp) {
						m_tail = (current == &m_head) ? nullptr : m_head;
						while (m_tail && m_tail->next) {
							m_tail = m_tail->next;
						}
					}
					m_allocator.destroy(temp);
					m_allocator.deallocate(temp, 1);
					m_size--;
					return;
				}
				current = &((*current)->next);
			}
		}

		bool Erase(ConstIterator it) {
			Node** current = &m_head;
			while (*current) {
				if (*current == it.m_node) {
					Node* temp = *current;
					*current   = (*current)->next;
					if (m_tail == temp) {
						m_tail = (current == &m_head) ? nullptr : m_head;
						while (m_tail && m_tail->next) {
							m_tail = m_tail->next;
						}
					}
					m_allocator.destroy(temp);
					m_allocator.deallocate(temp, 1);
					m_size--;
					return true;
				}
				current = &((*current)->next);
			}
			return false;
		}

		bool Erase(ConstIterator first, ConstIterator last) {
			Node** current = &m_head;
			while (*current) {
				if (*current == first.m_node) {
					while (*current != last.m_node) {
						Node* temp = *current;
						*current   = (*current)->next;
						if (m_tail == temp) {
							m_tail = (current == &m_head) ? nullptr : m_head;
							while (m_tail && m_tail->next) {
								m_tail = m_tail->next;
							}
						}
						m_allocator.destroy(temp);
						m_allocator.deallocate(temp, 1);
						m_size--;
					}
					return true;
				}
				current = &((*current)->next);
			}
			return false;
		}


		Reference At(size_t index) {
			if (index >= m_size) {
				LOG_FATAL("List Index out of bounds");
			}
			Node* current = m_head;
			for (size_t i = 0; i < index; ++i) {
				current = current->next;
			}
			return current->data;
		}

		ConstReference At(size_t index) const { return const_cast<List*>(this)->At(index); }

		Iterator      Begin() { return Iterator(m_head); }
		Iterator      End() { return Iterator(nullptr); }
		ConstIterator Begin() const { return ConstIterator(m_head); }
		ConstIterator End() const { return ConstIterator(nullptr); }

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

		Reference      Front() { return m_head->data; }
		ConstReference Front() const { return m_head->data; }

		Reference      Back() { return m_tail->data; }
		ConstReference Back() const { return m_tail->data; }

		bool   Empty() const { return m_size == 0; }
		size_t Size() const { return m_size; }

		bool operator==(const List& other) const {
			if (m_size != other.m_size)
				return false;
			Node* it1 = m_head;
			Node* it2 = other.m_head;
			while (it1) {
				if (it1->data != it2->data)
					return false;
				it1 = it1->next;
				it2 = it2->next;
			}
			return true;
		}

		bool operator!=(const List& other) const { return !(*this == other); }

		NodeAllocator GetAllocator() const { return m_allocator; }

	  private:
		Node*        m_head;
		Node*        m_tail;
		size_t       m_size;
		NodeAllocator m_allocator;
	};

	template<typename _Ty, typename Allocator> void Swap(List<_Ty, Allocator>& a, List<_Ty, Allocator>& b) { a.Swap(b); }
} // namespace Spark

#endif // SPARK_LIST_HPP