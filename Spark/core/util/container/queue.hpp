#ifndef SPARK_QUEUE_HPP
#define SPARK_QUEUE_HPP

#include <queue>
#include <iostream>
#include <core/util/types.hpp>
#include <core/util/log.hpp>
#include <core/util/classic/util.hpp>

namespace Spark {
	template<typename T> class Queue {
	public:
		using Pointer        = T*;
		using ConstPointer   = const T*;
		using Reference      = T&;
		using ConstReference = const T&;

		Queue()
			: m_data(nullptr)
			, m_front(0)
			, m_size(0)
			, m_capacity(0) {
			Reserve(1);
		}

		Queue(const std::queue<T>& other)
			: m_data(nullptr)
			, m_front(0)
			, m_size(other.size())
			, m_capacity(other.size()) {
			Reserve(other.size());
			std::queue<T> tmp = other;
			for (i32 i = 0; i < m_size; ++i) {
				m_data[i] = tmp.front();
				tmp.pop();
			}
		}

		explicit Queue(size_t initial_capacity)
			: m_data(nullptr)
			, m_front(0)
			, m_size(0)
			, m_capacity(initial_capacity) {
			Reserve(initial_capacity);
		}

		~Queue() {
			for (size_t i = 0; i < m_size; ++i) {
				m_data[(m_front + i) % m_capacity].~T();
			}
			::operator delete(m_data);
		}

		Queue(const Queue& other)
			: m_data(new T[other.m_capacity])
			, m_front(0)
			, m_size(other.m_size)
			, m_capacity(other.m_capacity) { for (size_t i = 0; i < m_size; ++i) { m_data[i] = other.m_data[(other.m_front + i) % other.m_capacity]; } }

		Queue& operator=(const Queue& other) {
			if (this != &other) {
				delete[] m_data;
				m_capacity = other.m_capacity;
				m_size     = other.m_size;
				m_front    = 0;
				m_data     = new T[m_capacity];
				for (size_t i = 0; i < m_size; ++i) { m_data[i] = other.m_data[(other.m_front + i) % other.m_capacity]; }
			}
			return *this;
		}

		void Enqueue(const T& value) {
			if (m_size == m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			new (m_data + (m_front + m_size) % m_capacity) T(value);
			++m_size;
		}

		void Enqueue(T&& value) {
			if (m_size == m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			new (m_data + (m_front + m_size) % m_capacity) T(Move(value));
			++m_size;
		}

		void Dequeue() {
			if (Empty()) {
				LOG_FATAL("Queue is empty");
			}
			m_data[m_front].~T();
			m_front = (m_front + 1) % m_capacity;
			--m_size;
		}

		Reference Front() {
			if (Empty()) { LOG_FATAL("Queue is empty"); }
			return m_data[m_front];
		}

		ConstReference Front() const {
			if (Empty()) { LOG_FATAL("Queue is empty"); }
			return m_data[m_front];
		}

		Reference Back() {
			if (Empty()) { LOG_FATAL("Queue is empty"); }
			return m_data[(m_front + m_size - 1) % m_capacity];
		}

		ConstReference Back() const {
			if (Empty()) { LOG_FATAL("Queue is empty"); }
			return m_data[(m_front + m_size - 1) % m_capacity];
		}

		friend std::ostream& operator<<(std::ostream& os, const Queue& queue) {
			std::cout << "[";
			for (size_t i = 0; i < queue.m_size; ++i) {
				std::cout << queue.m_data[(queue.m_front + i) % queue.m_capacity];
				if (i < queue.m_size - 1) { std::cout << ", "; }
				if (i == 10) {
					std::cout << "...";
					break;
				}
			}
			std::cout << "]";
			return os;
		}

		bool Empty() const { return m_size == 0; }

		size_t Size() const { return m_size; }

		size_t Capacity() const { return m_capacity; }

		void Clear() {
			m_front = 0;
			m_size  = 0;
		}

		void Reserve(size_t new_capacity) {
			if (new_capacity > m_capacity) {
				T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
				for (size_t i = 0; i < m_size; ++i) {
					new (new_data + i) T(Move(m_data[(m_front + i) % m_capacity]));
					m_data[(m_front + i) % m_capacity].~T();
				}
				::operator delete(m_data);
				m_data     = new_data;
				m_capacity = new_capacity;
				m_front    = 0;
			}
		}

		void ShrinkToFit() {
			if (m_size < m_capacity) {
				Pointer new_data = new T[m_size];
				for (size_t i = 0; i < m_size; ++i) { new_data[i] = m_data[(m_front + i) % m_capacity]; }
				delete[] m_data;
				m_data     = new_data;
				m_capacity = m_size;
				m_front    = 0;
			}
		}

		void Swap(Queue& other) {
			Spark::Swap(m_data, other.m_data);
			Spark::Swap(m_front, other.m_front);
			Spark::Swap(m_size, other.m_size);
			Spark::Swap(m_capacity, other.m_capacity);
		}



		template<typename... Args> void Emplace(Args&&... args) {
			if (m_size == m_capacity) { Reserve(m_capacity == 0 ? 1 : m_capacity * 2); }
			new(&m_data[(m_front + m_size) % m_capacity]) T(Forward<Args>(args)...);
			++m_size;
		}

	private:
		Pointer m_data;
		size_t  m_front;
		size_t  m_size;
		size_t  m_capacity;
	};

	template<typename T> void Swap(Queue<T>& lhs, Queue<T>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_QUEUE_HPP