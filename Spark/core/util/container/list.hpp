#ifndef SPARK_LIST_HPP
#define SPARK_LIST_HPP

#include <core/util/types.hpp>

namespace Spark {
	template<typename T> class List {
	public:
		using Pointer        = T*;
		using ConstPointer   = const T*;
		using Reference      = T&;
		using ConstReference = const T&;

		class Node {
			Pointer data;
			Node*   next;
		};

		using NodePtr = Node*;

		class Iterator {
		public:
			using ValueType      = T;
			using DifferenceType = i64;

			Iterator(Pointer ptr) : m_ptr(ptr) {}

			Reference operator*() const { return *m_ptr; }
			Pointer   operator->() { return m_ptr; }

			Iterator& operator++() {
				m_ptr++;
				return *this;
			}

			Iterator operator++(i32) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			Iterator& operator--() {
				m_ptr--;
				return *this;
			}

			Iterator operator--(i32) {
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}

			Iterator operator+(DifferenceType n) const { return Iterator(m_ptr + n); }
			Iterator operator-(DifferenceType n) const { return Iterator(m_ptr - n); }

			DifferenceType operator-(const Iterator& other) const { return m_ptr - other.m_ptr; }

			Reference operator[](DifferenceType n) const { return *(m_ptr + n); }

			bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
			bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
			bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
			bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
			bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }
			bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }

		private:
			Pointer m_ptr;
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
			NodePtr current = other.m_head;
			while (current) {
				PushBack(*current->data);
				current = current->next;
			}
		}

		List(List&& other) noexcept
			: m_head(other.m_head)
			, m_tail(other.m_tail)
			, m_size(other.m_size) {
			other.m_head = nullptr;
			other.m_tail = nullptr;
			other.m_size = 0;
		}

		List(const std::initializer_list<T>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) { for (ConstReference value : values) { PushBack(value); } }

		List(const std::vector<T>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) { for (ConstReference value : values) { PushBack(value); } }

		List(const std::list<T>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) { for (ConstReference value : values) { PushBack(value); } }

		~List() { Clear(); }

		List& operator=(const List& other) {
			if (this != &other) {
				Clear();
				NodePtr current = other.m_head;
				while (current) {
					PushBack(*current->data);
					current = current->next;
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
				other.m_head = nullptr;
				other.m_tail = nullptr;
				other.m_size = 0;
			}
			return *this;
		}

		void PushBack(ConstReference value) {
			NodePtr new_node = new Node();
			new_node->data   = new T(value);
			new_node->next   = nullptr;

			if (m_tail) {
				m_tail->next = new_node;
				m_tail       = new_node;
			} else {
				m_head = new_node;
				m_tail = new_node;
			}
			m_size++;
		}

		void PushFront(ConstReference value) {
			NodePtr new_node = new Node();
			new_node->data   = new T(value);
			new_node->next   = m_head;

			if (m_head) { m_head = new_node; } else {
				m_head = new_node;
				m_tail = new_node;
			}
			m_size++;
		}

		void PopBack() {
			if (m_size == 0) { return; }
			if (m_size == 1) {
				delete m_head->data;
				delete m_head;
				m_head = nullptr;
				m_tail = nullptr;
				m_size = 0;
				return;
			}

			NodePtr current = m_head;
			while (current->next != m_tail) { current = current->next; }
			delete m_tail->data;
			delete m_tail;
			m_tail       = current;
			m_tail->next = nullptr;
			m_size--;
		}

		void PopFront() {
			if (m_size == 0) { return; }
			if (m_size == 1) {
				delete m_head->data;
				delete m_head;
				m_head = nullptr;
				m_tail = nullptr;
				m_size = 0;
				return;
			}

			NodePtr current = m_head;
			m_head          = m_head->next;
			delete current->data;
			delete current;
			m_size--;
		}

		void Clear() {
			NodePtr current = m_head;
			while (current) {
				NodePtr next = current->next;
				delete current->data;
				delete current;
				current = next;
			}
			m_head = nullptr;
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
			if (m_size <= 1) { return; }
			NodePtr new_tail = m_head;
			m_head           = m_head->next;
			m_tail->next     = new_tail;
			m_tail           = new_tail;
			m_tail->next     = nullptr;
		}

		void RotateRight() {
			if (m_size <= 1) { return; }
			NodePtr new_head = m_tail;
			NodePtr current  = m_head;
			while (current->next != m_tail) { current = current->next; }
			m_tail         = current;
			m_tail->next   = nullptr;
			new_head->next = m_head;
			m_head         = new_head;
		}

		void Sort() {
			if (m_size <= 1) { return; }
			NodePtr current = m_head;
			while (current) {
				NodePtr min  = current;
				NodePtr next = current->next;
				while (next) {
					if (*next->data < *min->data) { min = next; }
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
				if (*current->data == value) {
					if (prev) {
						prev->next = current->next;
						delete current->data;
						delete current;
						current = prev->next;
					} else {
						NodePtr next = current->next;
						delete current->data;
						delete current;
						m_head  = next;
						current = m_head;
					}
					m_size--;
				} else {
					prev    = current;
					current = current->next;
				}
			}
		}

		void Erase(size_t index) {
			if (index >= m_size) { return; }
			if (index == 0) {
				NodePtr next = m_head->next;
				delete m_head->data;
				delete m_head;
				m_head = next;
				m_size--;
				return;
			}

			NodePtr current = m_head;
			NodePtr prev    = nullptr;
			size_t  i       = 0;
			while (current && i < index) {
				prev    = current;
				current = current->next;
				i++;
			}
			if (current) {
				prev->next = current->next;
				delete current->data;
				delete current;
				m_size--;
			}
		}

		Reference At(size_t index) {
			if (index >= m_size) { LOG_FATAL("Index out of bounds"); }
			NodePtr current = m_head;
			size_t  i       = 0;
			while (current && i < index) {
				current = current->next;
				i++;
			}
			return *current->data;
		}

		friend std::ostream& operator<<(std::ostream& os, const List& list) {
			os << "[ ";
			NodePtr current = list.m_head;
			size_t i       = 0;
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

		Iterator      Begin() { return Iterator(m_head->data); }
		Iterator      End() { return Iterator(m_tail->data); }
		ConstIterator Begin() const { return ConstIterator(m_head->data); }
		ConstIterator End() const { return ConstIterator(m_tail->data); }

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

		Reference      Back() { return *m_tail->data; }
		ConstReference Back() const { return *m_tail->data; }

		bool Empty() const { return m_size == 0; }

		bool Contains(ConstReference value) const {
			NodePtr current = m_head;
			while (current) {
				if (*current->data == value) { return true; }
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
			if (m_size != other.m_size) { return false; }
			NodePtr current = m_head;
			NodePtr other   = other.m_head;
			while (current && other) {
				if (*current->data != *other->data) { return false; }
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

		template<typename... Args> void EmplaceBack(Args&&... args) {
			NodePtr new_node = new Node();
			new_node->data   = new T(Forward<Args>(args)...);
			new_node->next   = nullptr;

			if (m_tail) {
				m_tail->next = new_node;
				m_tail       = new_node;
			} else {
				m_head = new_node;
				m_tail = new_node;
			}
			m_size++;
		}

		template<typename... Args> void EmplaceFront(Args&&... args) {
			NodePtr new_node = new Node();
			new_node->data   = new T(Forward<Args>(args)...);
			new_node->next   = m_head;

			if (m_head) { m_head = new_node; } else {
				m_head = new_node;
				m_tail = new_node;
			}
			m_size++;
		}

		template<typename... Args> void Emplace(size_t index, Args&&... args) {
			if (index >= m_size) { return; }
			if (index == 0) {
				EmplaceFront(Forward<Args>(args)...);
				return;
			}

			NodePtr new_node = new Node();
			new_node->data   = new T(Forward<Args>(args)...);

			NodePtr current = m_head;
			NodePtr prev    = nullptr;
			size_t  i       = 0;
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

		void operator+=(const std::initializer_list<T>& values) { for (ConstReference value : values) { PushBack(value); } }

		void operator+=(ConstReference value) { PushBack(value); }

		void operator-=(ConstReference value) { Remove(value); }

		bool operator!=(const List& other) const { return !(*this == other); }

		size_t Size() const { return m_size; }

	private:
		NodePtr m_head;
		NodePtr m_tail;
		size_t  m_size;
	};

	template<typename T> void Swap(List<T>& a, List<T>& b) { a.Swap(b); }
}

#endif