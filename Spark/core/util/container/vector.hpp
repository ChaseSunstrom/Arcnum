#ifndef SPARK_VECTOR_HPP
#define SPARK_VECTOR_HPP

#include <core/math/math.hpp>
#include <core/util/classic/util.hpp>
#include <core/util/log.hpp>
#include <core/util/memory/allocator.hpp>
#include <initializer_list>
#include <iostream>
#include <list>
#include <vector>

namespace Spark {
	/**
	 * @brief A custom vector implementation with allocator support.
	 * @tparam _Ty The type of elements stored in the vector.
	 * @tparam Allocator The allocator type used for memory management.
	 */
	template<typename _Ty, typename Allocator = _SPARK Allocator<_Ty>> class Vector {
	  public:
		using AllocatorType   = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using ValueType       = typename AllocatorTraits::ValueType;
		using Pointer         = typename AllocatorTraits::Pointer;
		using ConstPointer    = typename AllocatorTraits::ConstPointer;
		using Reference       = typename AllocatorTraits::Reference;
		using ConstReference  = typename AllocatorTraits::ConstReference;
		using SizeType        = typename AllocatorTraits::SizeType;
		using DifferenceType  = typename AllocatorTraits::DifferenceType;

		/**
		 * @brief Iterator class for the Vector.
		 */
		class Iterator {
		  public:
			using DifferenceType = ptrdiff;

			/**
			 * @brief Constructs an Iterator.
			 * @param ptr Pointer to the element.
			 */
			Iterator(Pointer ptr)
				: m_ptr(ptr) {}

			/**
			 * @brief Dereference operator.
			 * @return Reference to the element.
			 */
			Reference operator*() const { return *m_ptr; }

			/**
			 * @brief Arrow operator.
			 * @return Pointer to the element.
			 */
			Pointer   operator->() { return m_ptr; }

			/**
			 * @brief Pre-increment operator.
			 * @return Reference to the incremented iterator.
			 */
			Iterator& operator++() {
				m_ptr++;
				return *this;
			}

			/**
			 * @brief Post-increment operator.
			 * @return Copy of the iterator before increment.
			 */
			Iterator operator++(i32) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			/**
			 * @brief Pre-decrement operator.
			 * @return Reference to the decremented iterator.
			 */
			Iterator& operator--() {
				m_ptr--;
				return *this;
			}

			/**
			 * @brief Post-decrement operator.
			 * @return Copy of the iterator before decrement.
			 */
			Iterator operator--(i32) {
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}

			/**
			 * @brief Addition operator.
			 * @param n Number of positions to move forward.
			 * @return New iterator at the new position.
			 */
			Iterator operator+(DifferenceType n) const { return Iterator(m_ptr + n); }

			/**
			 * @brief Subtraction operator.
			 * @param n Number of positions to move backward.
			 * @return New iterator at the new position.
			 */
			Iterator operator-(DifferenceType n) const { return Iterator(m_ptr - n); }

			/**
			 * @brief Difference operator.
			 * @param other Another iterator to compare with.
			 * @return The distance between the two iterators.
			 */
			DifferenceType operator-(const Iterator& other) const { return m_ptr - other.m_ptr; }

			/**
			 * @brief Subscript operator.
			 * @param n Index to access.
			 * @return Reference to the element at the given index.
			 */
			Reference      operator[](DifferenceType n) const { return *(m_ptr + n); }

			/**
			 * @brief Equality comparison operator.
			 * @param other Another iterator to compare with.
			 * @return True if the iterators point to the same element, false otherwise.
			 */
			bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }

			/**
			 * @brief Inequality comparison operator.
			 * @param other Another iterator to compare with.
			 * @return True if the iterators point to different elements, false otherwise.
			 */
			bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }

			/**
			 * @brief Less than comparison operator.
			 * @param other Another iterator to compare with.
			 * @return True if this iterator points to an element before the other, false otherwise.
			 */
			bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }

			/**
			 * @brief Greater than comparison operator.
			 * @param other Another iterator to compare with.
			 * @return True if this iterator points to an element after the other, false otherwise.
			 */
			bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }

			/**
			 * @brief Less than or equal to comparison operator.
			 * @param other Another iterator to compare with.
			 * @return True if this iterator points to an element before or at the same position as the other, false otherwise.
			 */
			bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }

			/**
			 * @brief Greater than or equal to comparison operator.
			 * @param other Another iterator to compare with.
			 * @return True if this iterator points to an element after or at the same position as the other, false otherwise.
			 */
			bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }

		  private:
			Pointer m_ptr;
		};

		using ConstIterator = const Iterator;

		/**
		 * @brief Default constructor.
		 */
		Vector()
			: m_data(nullptr)
			, m_size(0)
			, m_capacity(0) {}

		/**
		 * @brief Constructor with initial size and value.
		 * @param initial_size The initial size of the vector.
		 * @param value The value to fill the vector with.
		 * @param allocator The allocator to use.
		 */
		Vector(SizeType initial_size, ConstReference value, AllocatorType allocator = AllocatorType())
			: m_allocator(allocator)
			, m_data(initial_size > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, initial_size * sizeof(_Ty))) : nullptr)
			, m_size(initial_size)
			, m_capacity(initial_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + i, value);
			}
		}

		/**
		 * @brief Constructor from std::vector.
		 * @param vec The std::vector to copy from.
		 */
		Vector(const std::vector<_Ty, AllocatorType>& vec)
			: m_allocator(vec.get_allocator())
			, m_data(vec.size() > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, vec.size() * sizeof(_Ty))) : nullptr)
			, m_size(vec.size())
			, m_capacity(vec.size()) {
			for (SizeType i = 0; i < m_size; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + i, vec[i]);
			}
		}

		/**
		 * @brief Constructor from std::list.
		 * @param list The std::list to copy from.
		 */
		Vector(const std::list<_Ty, AllocatorType>& list)
			: m_allocator(list.get_allocator())
			, m_data(list.size() > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, list.size() * sizeof(_Ty))) : nullptr)
			, m_size(list.size())
			, m_capacity(list.size()) {
			SizeType i = 0;
			for (const auto& item : list) {
				AllocatorTraits::Construct(m_allocator, m_data + i, item);
				++i;
			}
		}

		/**
		 * @brief Constructor with initial size and value.
		 * @param initial_size The initial size of the vector.
		 * @param value The value to fill the vector with.
		 */
		Vector(SizeType initial_size, Reference value)
			: m_data(initial_size > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, initial_size * sizeof(_Ty))) : nullptr)
			, m_size(initial_size)
			, m_capacity(initial_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + i, value);
			}
		}

		/**
		 * @brief Constructor from initializer list.
		 * @param list The initializer list to copy from.
		 */
		Vector(std::initializer_list<_Ty> list)
			: m_data(list.size() > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, list.size() * sizeof(_Ty))) : nullptr)
			, m_size(list.size())
			, m_capacity(list.size()) {
			SizeType i = 0;
			for (const auto& item : list) {
				AllocatorTraits::Construct(m_allocator, m_data + i, item);
				++i;
			}
		}

		/**
		 * @brief Constructor with initial size.
		 * @param initial_size The initial size of the vector.
		 */
		explicit Vector(SizeType initial_size)
			: m_data(initial_size > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, initial_size * sizeof(_Ty))) : nullptr)
			, m_size(initial_size)
			, m_capacity(initial_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + i);
			}
		}

		/**
		 * @brief Copy constructor.
		 * @param other The vector to copy from.
		 */
		Vector(const Vector& other)
			: m_allocator(other.m_allocator)
			, m_data(other.m_size > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, other.m_size * sizeof(_Ty))) : nullptr)
			, m_size(other.m_size)
			, m_capacity(other.m_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + i, other.m_data[i]);
			}
		}

		/**
		 * @brief Constructor from a range of iterators.
		 * @tparam InputIt The input iterator type.
		 * @param first Iterator to the first element of the range.
		 * @param last Iterator to one past the last element of the range.
		 */
		template<typename InputIt>
		Vector(InputIt first, InputIt last)
			: m_data(nullptr)
			, m_size(0)
			, m_capacity(0) {
			SizeType count = Distance(first, last);
			if (count > 0) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, count * sizeof(_Ty)));
				m_capacity = count;
				for (InputIt it = first; it != last; ++it) {
					AllocatorTraits::Construct(m_allocator, m_data + m_size, *it);
					++m_size;
				}
			}
		}

		/**
		 * @brief Move constructor.
		 * @param other The vector to move from.
		 */
		Vector(Vector&& other) noexcept
			: m_allocator(other.m_allocator)
			, m_data(other.m_data)
			, m_size(other.m_size)
			, m_capacity(other.m_capacity) {
			other.m_data     = nullptr;
			other.m_size     = 0;
			other.m_capacity = 0;
		}

		/**
		 * @brief Destructor.
		 */
		~Vector() {
			if (m_data) {
				for (SizeType i = 0; i < m_size; ++i) {
					AllocatorTraits::Destroy(m_allocator, m_data + i);
				}
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity * sizeof(_Ty));
				m_data = nullptr;
				m_size = 0;
				m_capacity = 0;
			}
		}

		/**
		 * @brief Copy assignment operator.
		 * @param other The vector to copy from.
		 * @return Reference to this vector.
		 */
		Vector& operator=(const Vector& other) {
			if (this != &other) {
				Clear();
				if (m_capacity < other.m_size) {
					AllocatorTraits::Deallocate(m_allocator, static_cast<void*>(m_data));
					m_data     = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, other.m_size * sizeof(_Ty)));
					m_capacity = other.m_size;
				}
				m_size = other.m_size;
				for (SizeType i = 0; i < m_size; ++i) {
					AllocatorTraits::Construct(m_allocator, m_data + i, other.m_data[i]);
				}
			}
			return *this;
		}

		/**
		 * @brief Move assignment operator.
		 * @param other The vector to move from.
		 * @return Reference to this vector.
		 */
		Vector& operator=(Vector&& other) noexcept {
			if (this != &other) {
				Clear();
				AllocatorTraits::Deallocate(m_allocator, static_cast<void*>(m_data));
				m_data           = other.m_data;
				m_size           = other.m_size;
				m_capacity       = other.m_capacity;
				other.m_data     = nullptr;
				other.m_size     = 0;
				other.m_capacity = 0;
			}
			return *this;
		}

		/**
		 * @brief Inserts an element at the front of the vector.
		 * @param value The value to insert.
		 */
		void PushFront(ConstReference value) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			CopyBackwards(m_data, m_data + m_size, m_data + m_size + 1);
			m_data[0] = value;
			++m_size;
		}

		/**
		 * @brief Accesses an element at the specified index with bounds checking.
		 * @param index The index of the element to access.
		 * @return Reference to the element at the specified index.
		 */
		Reference At(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			return m_data[index];
		}

		/**
		 * @brief Accesses an element at the specified index with bounds checking (const version).
		 * @param index The index of the element to access.
		 * @return Const reference to the element at the specified index.
		 */
		ConstReference At(SizeType index) const {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			return m_data[index];
		}

		/**
		 * @brief Accesses an element at the specified index without bounds checking.
		 * @param index The index of the element to access.
		 * @return Reference to the element at the specified index.
		 */
		Reference operator[](SizeType index) { return m_data[index]; }

		/**
		 * @brief Accesses an element at the specified index without bounds checking (const version).
		 * @param index The index of the element to access.
		 * @return Const reference to the element at the specified index.
		 */
		ConstReference operator[](SizeType index) const { return m_data[index]; }

		/**
		 * @brief Equality comparison operator.
		 * @param other The vector to compare with.
		 * @return True if the vectors are equal, false otherwise.
		 */
		bool operator==(const Vector& other) const {
			if (m_size != other.m_size) {
				return false;
			}
			for (SizeType i = 0; i < m_size; ++i) {
				if (m_data[i] != other.m_data[i]) {
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief Inequality comparison operator.
		 * @param other The vector to compare with.
		 * @return True if the vectors are not equal, false otherwise.
		 */
		bool operator!=(const Vector& other) const { return !(*this == other); }

		/**
		 * @brief Less than comparison operator.
		 * @param other The vector to compare with.
		 * @return True if this vector is lexicographically less than the other, false otherwise.
		 */
		bool operator<(const Vector& other) const {
			SizeType min_size = _MATH Min(m_size, other.m_size);
			for (SizeType i = 0; i < min_size; ++i) {
				if (m_data[i] < other.m_data[i]) {
					return true;
				}
				if (m_data[i] > other.m_data[i]) {
					return false;
				}
			}
			return m_size < other.m_size;
		}

		/**
		 * @brief Greater than comparison operator.
		 * @param other The vector to compare with.
		 * @return True if this vector is lexicographically greater than the other, false otherwise.
		 */
		bool operator>(const Vector& other) const {
			SizeType min_size = _MATH Min(m_size, other.m_size);
			for (SizeType i = 0; i < min_size; ++i) {
				if (m_data[i] > other.m_data[i]) {
					return true;
				}
				if (m_data[i] < other.m_data[i]) {
					return false;
				}
			}
			return m_size > other.m_size;
		}

		/**
		 * @brief Less than or equal to comparison operator.
		 * @param other The vector to compare with.
		 * @return True if this vector is lexicographically less than or equal to the other, false otherwise.
		 */
		bool operator<=(const Vector& other) const { return !(*this > other); }

		/**
		 * @brief Greater than or equal to comparison operator.
		 * @param other The vector to compare with.
		 * @return True if this vector is lexicographically greater than or equal to the other, false otherwise.
		 */
		bool operator>=(const Vector& other) const { return !(*this < other); }

		/**
		 * @brief Checks if the vector contains a specific value.
		 * @param value The value to search for.
		 * @return True if the value is found, false otherwise.
		 */
		bool Contains(ConstReference value) const {
			for (SizeType i = 0; i < m_size; ++i) {
				if (m_data[i] == value) {
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Finds the index of a specific value in the vector.
		 * @param value The value to search for.
		 * @param index The index where the value is found (if found).
		 * @return True if the value is found, false otherwise.
		 */
		bool Find(ConstReference value, SizeType& index) const {
			for (SizeType i = 0; i < m_size; ++i) {
				if (m_data[i] == value) {
					index = i;
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Removes the first occurrence of a specific value from the vector.
		 * @param value The value to remove.
		 */
		void Remove(ConstReference value) {
			SizeType index;
			if (Find(value, index)) {
				Erase(index);
			}
		}

		/**
		 * @brief Sorts the vector in ascending order.
		 */
		void Sort() { Sort(m_data, m_data + m_size); }

		/**
		 * @brief Adds a value to the end of the vector.
		 * @param value The value to add.
		 */
		void operator+=(ConstReference value) { PushBack(value); }

		/**
		 * @brief Appends another vector to this vector.
		 * @param other The vector to append.
		 */
		void operator+=(const Vector& other) {
			Reserve(m_size + other.m_size);
			for (SizeType i = 0; i < other.m_size; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + m_size + i, other.m_data[i]);
			}
			m_size += other.m_size;
		}

		/**
		 * @brief Appends an initializer list to this vector.
		 * @param list The initializer list to append.
		 */
		void operator+=(std::initializer_list<_Ty> list) {
			Reserve(m_size + list.size());
			for (const auto& item : list) {
				AllocatorTraits::Construct(m_allocator, m_data + m_size, item);
				++m_size;
			}
		}

		/**
		 * @brief Appends a std::vector to this vector.
		 * @param vec The std::vector to append.
		 */
		void operator+=(const std::vector<_Ty, AllocatorType>& vec) {
			Reserve(m_size + vec.size());
			for (const auto& item : vec) {
				AllocatorTraits::Construct(m_allocator, m_data + m_size, item);
				++m_size;
			}
		}

		/**
		 * @brief Removes the first occurrence of a value from the vector.
		 * @param value The value to remove.
		 */
		void operator-=(ConstReference value) { Remove(value); }

		/**
		 * @brief Removes all occurrences of values from another vector.
		 * @param other The vector containing values to remove.
		 */
		void operator-=(const Vector& other) {
			for (SizeType i = 0; i < other.m_size; ++i) {
				Remove(other.m_data[i]);
			}
		}

		/**
		 * @brief Removes all occurrences of values from an initializer list.
		 * @param list The initializer list containing values to remove.
		 */
		void operator-=(std::initializer_list<_Ty> list) {
			for (const auto& item : list) {
				Remove(item);
			}
		}

		/**
		 * @brief Removes all occurrences of values from a std::vector.
		 * @param vec The std::vector containing values to remove.
		 */
		void operator-=(const std::vector<_Ty, AllocatorType>& vec) {
			for (const auto& item : vec) {
				Remove(item);
			}
		}

		/**
		 * @brief Inserts a value at a specific index.
		 * @param index The index at which to insert the value.
		 * @param value The value to insert.
		 */
		void Insert(SizeType index, ConstReference value) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			if (m_size == m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (m_data == nullptr) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + 1);
			m_data[index] = value;
			++m_size;
		}

		/**
		 * @brief Inserts a value at a specific position.
		 * @param position Iterator pointing to the insertion position.
		 * @param value The value to insert.
		 */
		void Insert(Iterator position, ConstReference value) { Insert(position - m_data, value); }

		/**
		 * @brief Inserts a value at a specific position.
		 * @param position Iterator pointing to the insertion position.
		 * @param value The value to insert.
		 */
		void Insert(Iterator position, Reference value) { Insert(position - m_data, value); }

		/**
		 * @brief Inserts a value at a specific index.
		 * @param index The index at which to insert the value.
		 * @param value The value to insert.
		 */
		void Insert(SizeType index, Reference value) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			if (m_size == m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (m_data == nullptr) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + 1);
			m_data[index] = value;
			++m_size;
		}

		/**
		 * @brief Inserts a range of elements at a specific position.
		 * @param position Iterator pointing to the insertion position.
		 * @param first Iterator to the start of the range to insert.
		 * @param last Iterator to the end of the range to insert.
		 */
		void Insert(Iterator position, Iterator first, Iterator last) { Insert(position - m_data, first, last); }

		/**
		 * @brief Inserts a range of elements at a specific index.
		 * @param index The index at which to insert the range.
		 * @param first Iterator to the start of the range to insert.
		 * @param last Iterator to the end of the range to insert.
		 */
		void Insert(SizeType index, Iterator first, Iterator last) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			SizeType count = last - first;
			if (m_size + count > m_capacity) {
				Reserve(m_size + count);
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + count);
			for (SizeType i = 0; i < count; ++i) {
				AllocatorTraits::Construct(m_allocator, m_data + index + i, *(first + i));
			}
			m_size += count;
		}

		/**
		 * @brief Erases an element at a specific position.
		 * @param position Iterator pointing to the element to erase.
		 */
		void Erase(Iterator position) { Erase(position - m_data); }

		/**
		 * @brief Erases an element at a specific index.
		 * @param index The index of the element to erase.
		 */
		void Erase(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			Copy(m_data + index + 1, m_data + m_size, m_data + index);
			--m_size;
		}

		/**
		 * @brief Outputs the vector to an output stream.
		 * @param os The output stream.
		 * @param vec The vector to output.
		 * @return The output stream.
		 */
		friend std::ostream& operator<<(std::ostream& os, const Vector& vec) {
			os << "[";
			for (SizeType i = 0; i < vec.m_size; ++i) {
				os << vec.m_data[i];
				if (i != vec.m_size - 1) {
					os << ", ";
				}
				if (i == 10) {
					os << "...";
					break;
				}
			}
			os << "]";
			return os;
		}

		/**
		 * @brief Returns the size of the vector.
		 * @return The number of elements in the vector.
		 */
		SizeType Size() const { return m_size; }

		/**
		 * @brief Returns the capacity of the vector.
		 * @return The current capacity of the vector.
		 */
		SizeType Capacity() const { return m_capacity; }

		/**
		 * @brief Checks if the vector is empty.
		 * @return True if the vector is empty, false otherwise.
		 */
		bool Empty() const { return m_size == 0; }

		/**
		 * @brief Clears the contents of the vector.
		 */
		void Clear() {
			for (SizeType i = 0; i < m_size; ++i) {
				m_data[i].~_Ty();
			}
			m_size = 0;
		}

		/**
		 * @brief Adds an element to the end of the vector.
		 * @param value The value to add.
		 */
		void PushBack(ConstReference value) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			AllocatorTraits::Construct(m_allocator, m_data + m_size, value);
			++m_size;
		}

		/**
		 * @brief Adds an element to the end of the vector using move semantics.
		 * @param value The value to add.
		 */
		void PushBack(_Ty&& value) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			AllocatorTraits::Construct(m_allocator, m_data + m_size, Move(value));
			++m_size;
		}

		/**
		 * @brief Removes the last element from the vector.
		 */
		void PopBack() {
			if (m_size > 0) {
				--m_size;
				AllocatorTraits::Destroy(m_allocator, m_data + m_size);
			}
		}

		/**
		 * @brief Shrinks the vector to fit its size.
		 */
		void ShrinkToFit() {
			if (m_size < m_capacity) {
				_Ty* new_data = m_size > 0 ? static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, m_size * sizeof(_Ty))) : nullptr;
				for (SizeType i = 0; i < m_size; ++i) {
					AllocatorTraits::Construct(m_allocator, new_data + i, Move(m_data[i]));
					AllocatorTraits::Destroy(m_allocator, m_data + i);
				}
				AllocatorTraits::Deallocate(m_allocator, static_cast<void*>(m_data));
				m_data     = new_data;
				m_capacity = m_size;
			}
		}

		/**
		 * @brief Swaps the contents of two vectors.
		 * @param other The vector to swap with.
		 */
		void Swap(Vector& other) {
			_SPARK Swap(m_data, other.m_data);
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_capacity, other.m_capacity);
		}

		/**
		 * @brief Accesses the first element of the vector.
		 * @return Reference to the first element.
		 */
		Reference Front() { return m_data[0]; }

		/**
		 * @brief Accesses the first element of the vector (const version).
		 * @return Const reference to the first element.
		 */
		ConstReference Front() const { return m_data[0]; }

		/**
		 * @brief Accesses the middle element of the vector.
		 * @return Reference to the middle element.
		 */
		Reference      Middle() { return m_data[m_size / 2]; }

		/**
		 * @brief Accesses the middle element of the vector (const version).
		 * @return Const reference to the middle element.
		 */
		ConstReference Middle() const { return m_data[m_size / 2]; }

		/**
		 * @brief Accesses the last element of the vector.
		 * @return Reference to the last element.
		 */
		Reference      Back() { return m_data[m_size - 1]; }

		/**
		 * @brief Accesses the last element of the vector (const version).
		 * @return Const reference to the last element.
		 */
		ConstReference Back() const { return m_data[m_size - 1]; }

		/**
		 * @brief Accesses an element at a specific index.
		 * @param index The index of the element to access.
		 * @return Reference to the element at the specified index.
		 */
		Reference      Get(SizeType index) { return m_data[index]; }

		/**
		 * @brief Accesses an element at a specific index (const version).
		 * @param index The index of the element to access.
		 * @return Const reference to the element at the specified index.
		 */
		ConstReference Get(SizeType index) const { return m_data[index]; }

		/**
		 * @brief Returns a pointer to the data of the vector.
		 * @return Pointer to the data of the vector.
		 */
		Pointer      Data() { return m_data; }

		/**
		 * @brief Returns a pointer to the data of the vector (const version).
		 * @return Const pointer to the data of the vector.
		 */
		ConstPointer Data() const { return m_data; }

		/**
		 * @brief Returns the allocator used by the vector.
		 * @return The allocator used by the vector.
		 */
		Allocator GetAllocator() const { return m_allocator; }

		/**
		 * @brief Reserves a new capacity for the vector.
		 * @param new_capacity The new capacity to reserve.
		 */
		void Reserve(SizeType new_capacity) {
			if (new_capacity > m_capacity) {
				_Ty* new_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, new_capacity * sizeof(_Ty)));
				if (m_data) {
					for (SizeType i = 0; i < m_size; ++i) {
						AllocatorTraits::Construct(m_allocator, new_data + i, Move(m_data[i]));
						AllocatorTraits::Destroy(m_allocator, m_data + i);
					}
					AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity * sizeof(_Ty));
				}
				m_data     = new_data;
				m_capacity = new_capacity;
			}
		}

		/**
		 * @brief Swaps and pops an element at a specific index.
		 * @param index The index of the element to swap and pop.
		 */
		void SwapAndPop(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			_SPARK Swap(m_data[index], m_data[m_size - 1]);
			PopBack();
		}

		/**
		 * @brief Resizes the vector to a new size.
		 * @param new_size The new size of the vector.
		 */
		void Resize(SizeType new_size) {
			if (new_size > m_capacity) {
				Reserve(new_size);
			}
			m_size = new_size;
		}

		/**
		 * @brief Emplaces a new element at the end of the vector.
		 * @param args The arguments to construct the new element.
		 */
		template<typename... Args> void EmplaceBack(Args&&... args) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			AllocatorTraits::Construct(m_allocator, m_data + m_size, Forward<Args>(args)...);
			++m_size;
		}

		/**
		 * @brief Emplaces a new element at the front of the vector.
		 * @param args The arguments to construct the new element.
		 */
		template<typename... Args> void EmplaceFront(Args&&... args) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			CopyBackwards(m_data, m_data + m_size, m_data + m_size + 1);
			AllocatorTraits::Construct(m_allocator, m_data, Forward<Args>(args)...);
			++m_size;
		}

		/**
		 * @brief Emplaces a new element at a specific index.
		 * @param index The index at which to emplace the new element.
		 * @param args The arguments to construct the new element.
		 */
		template<typename... Args> void Emplace(SizeType index, Args&&... args) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(AllocatorTraits::Allocate(m_allocator, sizeof(_Ty)));
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + 1);
			AllocatorTraits::Construct(m_allocator, m_data + index, Forward<Args>(args)...);
			++m_size;
		}

		/**
		 * @brief Returns an iterator to the beginning of the vector.
		 * @return Iterator to the beginning of the vector.
		 */
		Iterator      Begin() { return Iterator(m_data); }

		/**
		 * @brief Returns an iterator to the end of the vector.
		 * @return Iterator to the end of the vector.
		 */
		Iterator      End() { return Iterator(m_data + m_size); }

		/**
		 * @brief Returns a const iterator to the beginning of the vector.
		 * @return Const iterator to the beginning of the vector.
		 */
		ConstIterator Begin() const { return Iterator(m_data); }

		/**
		 * @brief Returns a const iterator to the end of the vector.
		 * @return Const iterator to the end of the vector.
		 */
		ConstIterator End() const { return Iterator(m_data + m_size); }

		/**
		 * @brief Returns an iterator to the beginning of the vector.
		 * @return Iterator to the beginning of the vector.
		 */
		Iterator      begin() { return Begin(); }

		/**
		 * @brief Returns an iterator to the end of the vector.
		 * @return Iterator to the end of the vector.
		 */
		Iterator      end() { return End(); }

		/**
		 * @brief Returns a const iterator to the beginning of the vector.
		 * @return Const iterator to the beginning of the vector.
		 */
		ConstIterator begin() const { return Begin(); }

		/**
		 * @brief Returns a const iterator to the end of the vector.
		 * @return Const iterator to the end of the vector.
		 */
		ConstIterator end() const { return End(); }

	  private:
		/**
		 * @brief Copies elements from a source range to a destination range in reverse order.
		 * @param src_begin Iterator to the start of the source range.
		 * @param src_end Iterator to the end of the source range.
		 * @param dest_end Iterator to the start of the destination range.
		 */
		void CopyBackwards(Pointer src_begin, Pointer src_end, Pointer dest_end) {
			while (src_end != src_begin) {
				--src_end;
				--dest_end;
				AllocatorTraits::Construct(m_allocator, dest_end, Move(*src_end));
				src_end->~_Ty();
			}
		}

		/**
		 * @brief Copies elements from a source range to a destination range.
		 * @param src_begin Iterator to the start of the source range.
		 * @param src_end Iterator to the end of the source range.
		 * @param dest_begin Iterator to the start of the destination range.
		 */
		void Copy(Pointer src_begin, Pointer src_end, Pointer dest_begin) {
			while (src_begin != src_end) {
				AllocatorTraits::Construct(m_allocator, dest_begin, Move(*src_begin));
				src_begin->~_Ty();
				++src_begin;
				++dest_begin;
			}
		}

	  private:
		Pointer       m_data;
		AllocatorType m_allocator;
		SizeType      m_size;
		SizeType      m_capacity;
	};

	/**
	 * @brief Swaps two vectors.
	 * @param lhs The first vector to swap.
	 * @param rhs The second vector to swap.
	 */
	template<typename _Ty> void Swap(Vector<_Ty>& lhs, Vector<_Ty>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_VECTOR_HPP