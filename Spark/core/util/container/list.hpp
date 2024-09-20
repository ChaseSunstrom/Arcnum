#ifndef SPARK_LIST_HPP
#define SPARK_LIST_HPP

#include <core/util/types.hpp>
#include <initializer_list>
#include <stdexcept>

namespace Spark {
	template<typename _Ty> class List {
	  private:
		struct Node {
			_Ty     data;
			Node* next;
			Node(const _Ty& value)
				: data(value)
				, next(nullptr) {}
			Node(_Ty&& value)
				: data(Move(value))
				, next(nullptr) {}
		};

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

		List()
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {}

		List(const List& other)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {
			for (const auto& value : other) {
				PushBack(value);
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

		List(const std::initializer_list<_Ty>& values)
			: m_head(nullptr)
			, m_tail(nullptr)
			, m_size(0) {
			for (const auto& value : values) {
				PushBack(value);
			}
		}

		~List() { Clear(); }

		List& operator=(const List& other) {
			if (this != &other) {
				Clear();
				for (const auto& value : other) {
					PushBack(value);
				}
			}
			return *this;
		}

		List& operator=(List&& other) noexcept {
			if (this != &other) {
				if (m_head)
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

		void PushFront(const _Ty& value) {
			Node* new_node = new Node(value);
			new_node->next = m_head;
			m_head         = new_node;
			if (!m_tail) {
				m_tail = m_head;
			}
			m_size++;
		}

		void PushFront(_Ty&& value) {
			Node* new_node = new Node(Move(value));
			new_node->next = m_head;
			m_head         = new_node;
			if (!m_tail) {
				m_tail = m_head;
			}
			m_size++;
		}

		void PopFront() {
			if (m_size == 0)
				return;
			Node* temp = m_head;
			m_head     = m_head->next;
			delete temp;
			m_size--;
			if (m_size == 0) {
				m_tail = nullptr;
			}
		}

		void PushBack(const _Ty& value) {
			Node* new_node = new Node(value);
			if (m_tail) {
				m_tail->next = new_node;
				m_tail       = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			m_size++;
		}

		void PushBack(_Ty&& value) {
			Node* new_node = new Node(Move(value));
			if (m_tail) {
				m_tail->next = new_node;
				m_tail       = new_node;
			} else {
				m_head = m_tail = new_node;
			}
			m_size++;
		}

		void PopBack() {
			if (m_size == 0)
				return;
			if (m_size == 1) {
				delete m_head;
				m_head = m_tail = nullptr;
				m_size          = 0;
				return;
			}
			Node* current = m_head;
			while (current->next != m_tail) {
				current = current->next;
			}
			delete m_tail;
			m_tail       = current;
			m_tail->next = nullptr;
			m_size--;
		}

		void Clear() {
			while (m_head) {
				Node* temp = m_head;
				m_head     = m_head->next;
				delete temp;
			}
			m_tail = nullptr;
			m_size = 0;
		}

		void Swap(List& other) {
			std::swap(m_head, other.m_head);
			std::swap(m_tail, other.m_tail);
			std::swap(m_size, other.m_size);
		}

		void Remove(const _Ty& value) {
			Node** current = &m_head;
			while (*current) {
				if ((*current)->data == value) {
					Node* temp = *current;
					*current   = (*current)->next;
					if (m_tail == temp) {
						m_tail = (current == &m_head) ? nullptr : m_head;
						while (m_tail && m_tail->next) {
							m_tail = m_tail->next;
						}
					}
					delete temp;
					m_size--;
					return;
				}
				current = &((*current)->next);
			}
		}

		Reference At(size_t  index) {
			if (index >= m_size) {
				throw std::out_of_range("Index out of bounds");
			}
			Node* current = m_head;
			for (size_t  i = 0; i < index; ++i) {
				current = current->next;
			}
			return current->data;
		}

		ConstReference At(size_t  index) const { return const_cast<List*>(this)->At(index); }

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
		size_t  Size() const { return m_size; }

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

	  private:
		Node*  m_head;
		Node*  m_tail;
		size_t  m_size;
	};

	template<typename _Ty> void Swap(List<_Ty>& a, List<_Ty>& b) { a.Swap(b); }
} // namespace Spark

#endif // SPARK_LIST_HPP