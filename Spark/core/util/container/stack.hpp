#ifndef SPARK_STACK_HPP
#define SPARK_STACK_HPP

#include <stack>
#include <iostream>
#include <core/util/types.hpp>
#include <core/util/log.hpp>
#include <core/util/classic/util.hpp>

namespace Spark {
	template<typename T> class Stack {
	public:
		using Pointer        = T*;
		using ConstPointer   = const T*;
		using Reference      = T&;
		using ConstReference = const T&;

		Stack()
			: m_data(nullptr)
			, m_size(0)
			, m_capacity(0) {}

		Stack(const std::stack<T>& other)
			: m_data(new T[other.size()])
			, m_size(other.size())
			, m_capacity(other.size()) {
			std::stack<T> tmp = other;
			for (i32 i = m_size - 1; i >= 0; --i) {
				m_data[i] = tmp.top();
				tmp.pop();
			}
		}

		explicit Stack(size_t initial_capacity)
			: m_data(new T[initial_capacity])
			, m_size(0)
			, m_capacity(initial_capacity) {}

		~Stack() { delete[] m_data; }

		Stack(const Stack& other)
			: m_data(new T[other.m_capacity])
			, m_size(other.m_size)
			, m_capacity(other.m_capacity) { Copy(other.m_data, other.m_data + m_size, m_data); }

		Stack& operator=(const Stack& other) {
			if (this != &other) {
				delete[] m_data;
				m_size     = other.m_size;
				m_capacity = other.m_capacity;
				m_data     = new T[m_capacity];
				Copy(other.m_data, other.m_data + m_size, m_data);
			}
			return *this;
		}

		void Push(ConstReference value) {
			if (m_size == m_capacity) { Reserve(m_capacity == 0 ? 1 : m_capacity * 2); }
			m_data[m_size++] = value;
		}

		void Pop() {
			if (Empty()) { LOG_FATAL("Stack is empty"); }
			--m_size;
		}

		Reference Top() {
			if (Empty()) { LOG_FATAL("Stack is empty"); }
			return m_data[m_size - 1];
		}

		ConstReference Top() const {
			if (Empty()) { LOG_FATAL("Stack is empty"); }
			return m_data[m_size - 1];
		}

		bool Empty() const { return m_size == 0; }

		size_t Size() const { return m_size; }

		size_t Capacity() const { return m_capacity; }

		void Clear() { m_size = 0; }

		void Reserve(size_t new_capacity) {
			if (new_capacity > m_capacity) {
				Pointer new_data = new T[new_capacity];
				Copy(m_data, m_data + m_size, new_data);
				delete[] m_data;
				m_data     = new_data;
				m_capacity = new_capacity;
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const Stack& stack) {
			os << "[";
			for (size_t i = 0; i < stack.m_size; ++i) {
				os << stack.m_data[i];
				if (i < stack.m_size - 1) { os << ", "; }
				if (i == 10) {
					os << "...";
					break;
				}
			}
			os << "]";
			return os;
		}

		void ShrinkToFit() {
			if (m_size < m_capacity) {
				Pointer new_data = new T[m_size];
				Copy(m_data, m_data + m_size, new_data);
				delete[] m_data;
				m_data     = new_data;
				m_capacity = m_size;
			}
		}

		void Swap(Stack& other) {
			Spark::Swap(m_data, other.m_data);
			Spark::Swap(m_size, other.m_size);
			Spark::Swap(m_capacity, other.m_capacity);
		}

		template<typename... Args> void Emplace(Args&&... args) {
			if (m_size == m_capacity) { Reserve(m_capacity == 0 ? 1 : m_capacity * 2); }
			new(&m_data[m_size]) T(Forward<Args>(args)...);
			++m_size;
		}

	private:
		Pointer m_data;
		size_t  m_size;
		size_t  m_capacity;
	};

	template<typename T> void Swap(Stack<T>& lhs, Stack<T>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_STACK_HPP