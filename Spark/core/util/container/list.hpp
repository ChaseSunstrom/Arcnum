#ifndef SPARK_LIST_HPP
#define SPARK_LIST_HPP

#include <core/util/types.hpp>
#include <core/util/memory/unique_ptr.hpp>

namespace Spark {
	template<typename T> class List {
	  private:
		struct Node {
			T     data;
			Node* next;
			Node(const T& value)
				: data(value)
				, next(nullptr) {}
			Node(T&& value)
				: data(Move(value))
				, next(nullptr) {}
		};

	  public:
		using Pointer        = T*;
		using ConstPointer   = const T*;
		using Reference      = T&;
		using ConstReference = const T&;
		using NodePtr        = UniquePtr<Node>;

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

			Iterator operator++(i32) {
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

		List()
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {}

		List(const List& other)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {
			Node* current = other.m_head.Get();
			while (current) {
				PushBack(current->data);
				current = current->next.Get();
			}
		}

		List(List&& other) noexcept
			: m_head(Move(other.m_head))
			, m_tail(other.m_tail)
			, m_size(other.m_size) {
			other.m_tail = nullptr;
			other.m_size = 0;
		}

		List(const std::initializer_list<T>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {
			for (ConstReference value : values) {
				PushBack(value);
			}
		}

		List(const std::vector<T>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {
			for (ConstReference value : values) {
				PushBack(value);
			}
		}

		List(const std::list<T>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {
			for (ConstReference value : values) {
				PushBack(value);
			}
		}

		~List() { Clear(); }

		List& operator=(const List& other) {
			if (this != &other) {
				Clear();
				Node* current = other.m_head.Get();
				while (current) {
					PushBack(current->data);
					current = current->next.Get();
				}
			}
			return *this;
		}

		List& operator=(List&& other) noexcept {
			if (this != &other) {
				Clear();
				m_head       = Move(other.m_head);
				m_tail       = other.m_tail;
				m_size       = other.m_size;
				other.m_tail = nullptr;
				other.m_size = 0;
			}
			return *this;
		}


		void PushFront(ConstReference value) {
			NodePtr new_node = MakeUnique<Node>(value);
			new_node->next   = Move(m_head);
			m_head           = Move(new_node);
			if (!m_tail) {
				m_tail = m_head.Get();
			}
			m_size++;
		}

		void PopFront() {
			if (m_size == 0) {
				return;
			}
			m_head = Move(m_head->next);
			m_size--;
			if (m_size == 0) {
				m_tail = nullptr;
			}
		}

		void PopBack() {
			if (m_size == 0) {
				return;
			}
			if (m_size == 1) {
				m_head.Reset();
				m_tail = nullptr;
				m_size = 0;
				return;
			}

			Node* current = m_head.Get();
			while (current->next.Get() != m_tail) {
				current = current->next.Get();
			}
			current->next.Reset();
			m_tail = current;
			m_size--;
		}

		
		void Clear() {
			m_head.Reset();
			m_tail = nullptr;
			m_size = 0;
		}


		void Swap(List& other) {
			Spark::Swap(m_head, other.m_head);
			Spark::Swap(m_tail, other.m_tail);
			Spark::Swap(m_size, other.m_size);
		}

		void Reverse() {
			NodePtr current = m_head;
			NodePtr prev    = nullptr;
			NodePtr next    = nullptr;
			while (current) {
				next          = current->next;
				current->next = prev;
				prev          = current;
				current       = next;
			}
			m_tail = m_head;
			m_head = prev;
		}

		void RotateLeft() {
			if (m_size <= 1) {
				return;
			}
			NodePtr new_tail = m_head;
			m_head           = m_head->next;
			m_tail->next     = new_tail;
			m_tail           = new_tail;
			m_tail->next     = nullptr;
		}

		void RotateRight() {
			if (m_size <= 1) {
				return;
			}
			NodePtr new_head = m_tail;
			NodePtr current  = m_head;
			while (current->next != m_tail) {
				current = current->next;
			}
			m_tail         = current;
			m_tail->next   = nullptr;
			new_head->next = m_head;
			m_head         = new_head;
		}

		void Sort() {
			if (m_size <= 1) {
				return;
			}
			NodePtr current = m_head;
			while (current) {
				NodePtr min  = current;
				NodePtr next = current->next;
				while (next) {
					if (*next->data < *min->data) {
						min = next;
					}
					next = next->next;
				}
				Spark::Swap(current->data, min->data);
				current = current->next;
			}
		}

		void Remove(ConstReference value) {
			NodePtr current = m_head;
			NodePtr prev    = nullptr;
			while (current) {
				if (current->data == value) {
					if (prev) {
						prev->next = current->next;
					} else {
						m_head = current->next;
					}
					if (current == m_tail) {
						m_tail = prev;
					}
					delete current; // This is correct now
					m_size--;
					return;
				}
				prev    = current;
				current = current->next;
			}
		}

		Reference At(size_t index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of bounds");
			}
			Node*  current = m_head.Get();
			size_t i       = 0;
			while (current && i < index) {
				current = current->next.Get();
				i++;
			}
			return current->data;
		}

		friend std::ostream& operator<<(std::ostream& os, const List& list) {
			os << "[ ";
			NodePtr current = list.m_head;
			size_t  i       = 0;
			while (current) {
				os << *current->data << " ";
				current = current->next;
				i++;
				if (i == 10) {
					os << "...";
					break;
				}
			}
			os << "]";
			return os;
		}

		Iterator      Begin() { return Iterator(m_head.Get()); }
		Iterator      End() { return Iterator(nullptr); }
		ConstIterator Begin() const { return ConstIterator(m_head.Get()); }
		ConstIterator End() const { return ConstIterator(nullptr); }

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

		Reference      operator[](size_t index) { return At(index); }
		ConstReference operator[](size_t index) const { return At(index); }

		Reference      Front() { return *m_head->data; }
		ConstReference Front() const { return *m_head->data; }

		Reference Middle() {
			NodePtr slow = m_head;
			NodePtr fast = m_head;
			while (fast && fast->next) {
				slow = slow->next;
				fast = fast->next->next;
			}
			return *slow->data;
		}

		ConstReference Middle() const {
			NodePtr slow = m_head;
			NodePtr fast = m_head;
			while (fast && fast->next) {
				slow = slow->next;
				fast = fast->next->next;
			}
			return *slow->data;
		}

		Reference      Back() { return m_tail->data; }
		ConstReference Back() const { return *m_tail->data; }

		bool Empty() const { return m_size == 0; }

		bool Contains(ConstReference value) const {
			NodePtr current = m_head;
			while (current) {
				if (*current->data == value) {
					return true;
				}
				current = current->next;
			}
			return false;
		}

		bool Find(ConstReference value, size_t& index) const {
			NodePtr current = m_head;
			size_t  i       = 0;
			while (current) {
				if (*current->data == value) {
					index = i;
					return true;
				}
				current = current->next;
				i++;
			}
			return false;
		}

		bool operator==(const List& other) const {
			if (m_size != other.m_size) {
				return false;
			}
			NodePtr current = m_head;
			NodePtr other   = other.m_head;
			while (current && other) {
				if (*current->data != *other->data) {
					return false;
				}
				current = current->next;
				other   = other->next;
			}
			return true;
		}

		void operator+=(const List& other) {
			NodePtr current = other.m_head;
			while (current) {
				PushBack(*current->data);
				current = current->next;
			}
		}

		  void PushBack(const T& value) {
			NodePtr new_node = MakeUnique<Node>(value);
			if (m_tail) {
				m_tail->next = Move(new_node);
				m_tail       = m_tail->next.Get();
			} else {
				m_head = Move(new_node);
				m_tail = m_head.Get();
			}
			++m_size;
		}

		void PushBack(T&& value) {
			NodePtr new_node = MakeUnique<Node>(Move(value));
			if (m_tail) {
				m_tail->next = Move(new_node);
				m_tail       = m_tail->next.Get();
			} else {
				m_head = Move(new_node);
				m_tail = m_head.Get();
			}
			++m_size;
		}

		template<typename... Args> void EmplaceBack(Args&&... args) {
			Node* new_node = new Node(T(Forward<Args>(args)...));
			if (m_tail) {
				m_tail->next = new_node;
				m_tail       = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			++m_size;
		}

		void Erase(Iterator it) {
			if (it.m_node == m_head) {
				PopFront();
			} else {
				Node* prev = m_head;
				while (prev->next != it.m_node) {
					prev = prev->next;
				}
				prev->next = it.m_node->next;
				if (it.m_node == m_tail) {
					m_tail = prev;
				}
				delete it.m_node;
				--m_size;
			}
		}

		template<typename... Args> void EmplaceFront(Args&&... args) {
			NodePtr new_node = new Node(T(Forward<Args>(args)...));
			new_node->next   = m_head;

			if (!m_head) {
				m_tail = new_node;
			}
			m_head = new_node;
			m_size++;
		}

		template<typename... Args> void Emplace(size_t index, Args&&... args) {
			if (index >= m_size) {
				return;
			}
			if (index == 0) {
				EmplaceFront(Forward<Args>(args)...);
				return;
			}

			NodePtr new_node = new Node(T(Forward<Args>(args)...));

			NodePtr current  = m_head;
			NodePtr prev     = nullptr;
			size_t  i        = 0;
			while (current && i < index) {
				prev    = current;
				current = current->next;
				i++;
			}
			if (current) {
				prev->next     = new_node;
				new_node->next = current;
				m_size++;
			}
		}

		void operator+=(const std::initializer_list<T>& values) {
			for (ConstReference value : values) {
				PushBack(value);
			}
		}

		void operator+=(ConstReference value) { PushBack(value); }

		void operator-=(ConstReference value) { Remove(value); }

		bool operator!=(const List& other) const { return !(*this == other); }

		size_t Size() const { return m_size; }

	  private:
		NodePtr m_head;
		Node*   m_tail; // Raw pointer, as it doesn't own the node
		size_t  m_size;
	};

	template<typename T> void Swap(List<T>& a, List<T>& b) { a.Swap(b); }
} // namespace Spark

#endif