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

		class Iterator {
		  public:
			using DifferenceType = std::ptrdiff_t;

			Iterator(Pointer ptr)
				: m_ptr(ptr) {}

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
			Reference      operator[](DifferenceType n) const { return *(m_ptr + n); }

			bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
			bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
			bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
			bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
			bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }
			bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }

		  private:
			Pointer m_ptr;
		};

		using ConstIterator = const Iterator;

		Vector()
			: m_data(nullptr)
			, m_size(0)
			, m_capacity(0) {}

		Vector(SizeType initial_size, ConstReference value)
			: m_data(initial_size > 0 ? static_cast<_Ty*>(::operator new(initial_size * sizeof(_Ty))) : nullptr)
			, m_size(initial_size)
			, m_capacity(initial_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				new (m_data + i) _Ty(value);
			}
		}

		Vector(const std::vector<_Ty>& vec)
			: m_data(vec.size() > 0 ? static_cast<_Ty*>(::operator new(vec.size() * sizeof(_Ty))) : nullptr)
			, m_size(vec.size())
			, m_capacity(vec.size()) {
			for (SizeType i = 0; i < m_size; ++i) {
				new (m_data + i) _Ty(vec[i]);
			}
		}

		Vector(const std::list<_Ty>& list)
			: m_data(list.size() > 0 ? static_cast<_Ty*>(::operator new(list.size() * sizeof(_Ty))) : nullptr)
			, m_size(list.size())
			, m_capacity(list.size()) {
			SizeType i = 0;
			for (const auto& item : list) {
				new (m_data + i) _Ty(item);
				++i;
			}
		}

		Vector(SizeType initial_size, Reference value)
			: m_data(initial_size > 0 ? static_cast<_Ty*>(::operator new(initial_size * sizeof(_Ty))) : nullptr)
			, m_size(initial_size)
			, m_capacity(initial_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				new (m_data + i) _Ty(value);
			}
		}

		Vector(std::initializer_list<_Ty> list)
			: m_data(list.size() > 0 ? static_cast<_Ty*>(::operator new(list.size() * sizeof(_Ty))) : nullptr)
			, m_size(list.size())
			, m_capacity(list.size()) {
			SizeType i = 0;
			for (const auto& item : list) {
				new (m_data + i) _Ty(item);
				++i;
			}
		}

		explicit Vector(SizeType initial_size)
			: m_data(initial_size > 0 ? static_cast<_Ty*>(::operator new(initial_size * sizeof(_Ty))) : nullptr)
			, m_size(initial_size)
			, m_capacity(initial_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				new (m_data + i) _Ty();
			}
		}

		Vector(const Vector& other)
			: m_data(other.m_size > 0 ? static_cast<_Ty*>(::operator new(other.m_size * sizeof(_Ty))) : nullptr)
			, m_size(other.m_size)
			, m_capacity(other.m_size) {
			for (SizeType i = 0; i < m_size; ++i) {
				new (m_data + i) _Ty(other.m_data[i]);
			}
		}

		Vector(Vector&& other) noexcept
			: m_data(other.m_data)
			, m_size(other.m_size)
			, m_capacity(other.m_capacity) {
			other.m_data     = nullptr;
			other.m_size     = 0;
			other.m_capacity = 0;
		}

		~Vector() {
			if (m_data) {
				for (SizeType i = 0; i < m_size; ++i) {
					m_data[i].~_Ty();
				}
				::operator delete(static_cast<void*>(m_data));
				m_data = nullptr;
			}
		}

		Vector& operator=(const Vector& other) {
			if (this != &other) {
				Clear();
				if (m_capacity < other.m_size) {
					::operator delete(m_data);
					m_data     = static_cast<_Ty*>(::operator new(other.m_size * sizeof(_Ty)));
					m_capacity = other.m_size;
				}
				m_size = other.m_size;
				for (SizeType i = 0; i < m_size; ++i) {
					new (m_data + i) _Ty(other.m_data[i]);
				}
			}
			return *this;
		}

		Vector& operator=(Vector&& other) noexcept {
			if (this != &other) {
				Clear();
				::operator delete(m_data);
				m_data           = other.m_data;
				m_size           = other.m_size;
				m_capacity       = other.m_capacity;
				other.m_data     = nullptr;
				other.m_size     = 0;
				other.m_capacity = 0;
			}
			return *this;
		}

		void PushFront(ConstReference value) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			CopyBackwards(m_data, m_data + m_size, m_data + m_size + 1);
			m_data[0] = value;
			++m_size;
		}

		Reference At(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			return m_data[index];
		}

		ConstReference At(SizeType index) const {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			return m_data[index];
		}

		Reference operator[](SizeType index) { return m_data[index]; }

		ConstReference operator[](SizeType index) const { return m_data[index]; }

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

		bool operator!=(const Vector& other) const { return !(*this == other); }

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

		bool operator<=(const Vector& other) const { return !(*this > other); }

		bool operator>=(const Vector& other) const { return !(*this < other); }

		bool Contains(ConstReference value) const {
			for (SizeType i = 0; i < m_size; ++i) {
				if (m_data[i] == value) {
					return true;
				}
			}
			return false;
		}

		bool Find(ConstReference value, SizeType& index) const {
			for (SizeType i = 0; i < m_size; ++i) {
				if (m_data[i] == value) {
					index = i;
					return true;
				}
			}
			return false;
		}

		void Remove(ConstReference value) {
			SizeType index;
			if (Find(value, index)) {
				Erase(index);
			}
		}

		void Sort() { Sort(m_data, m_data + m_size); }

		void operator+=(ConstReference value) { PushBack(value); }

		void operator+=(const Vector& other) {
			Reserve(m_size + other.m_size);
			for (SizeType i = 0; i < other.m_size; ++i) {
				new (m_data + m_size + i) _Ty(other.m_data[i]);
			}
			m_size += other.m_size;
		}

		void operator+=(std::initializer_list<_Ty> list) {
			Reserve(m_size + list.size());
			for (const auto& item : list) {
				new (m_data + m_size) _Ty(item);
				++m_size;
			}
		}

		void operator+=(const std::vector<_Ty>& vec) {
			Reserve(m_size + vec.size());
			for (const auto& item : vec) {
				new (m_data + m_size) _Ty(item);
				++m_size;
			}
		}

		void operator-=(ConstReference value) { Remove(value); }

		void operator-=(const Vector& other) {
			for (SizeType i = 0; i < other.m_size; ++i) {
				Remove(other.m_data[i]);
			}
		}

		void operator-=(std::initializer_list<_Ty> list) {
			for (const auto& item : list) {
				Remove(item);
			}
		}

		void operator-=(const std::vector<_Ty>& vec) {
			for (const auto& item : vec) {
				Remove(item);
			}
		}

		void Insert(SizeType index, ConstReference value) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			if (m_size == m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (m_data == nullptr) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + 1);
			m_data[index] = value;
			++m_size;
		}

		void Insert(Iterator position, ConstReference value) { Insert(position - m_data, value); }

		void Insert(Iterator position, Reference value) { Insert(position - m_data, value); }

		void Insert(SizeType index, Reference value) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			if (m_size == m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (m_data == nullptr) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + 1);
			m_data[index] = value;
			++m_size;
		}

		void Insert(Iterator position, Iterator first, Iterator last) { Insert(position - m_data, first, last); }

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
				new (m_data + index + i) _Ty(*(first + i));
			}
			m_size += count;
		}

		void Erase(Iterator position) { Erase(position - m_data); }

		void Erase(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			Copy(m_data + index + 1, m_data + m_size, m_data + index);
			--m_size;
		}

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

		SizeType Size() const { return m_size; }

		SizeType Capacity() const { return m_capacity; }

		bool Empty() const { return m_size == 0; }

		void Clear() {
			for (SizeType i = 0; i < m_size; ++i) {
				m_data[i].~_Ty();
			}
			m_size = 0;
		}

		void PushBack(ConstReference value) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			new (m_data + m_size) _Ty(value);
			++m_size;
		}

		void PushBack(_Ty&& value) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			new (m_data + m_size) _Ty(Move(value));
			++m_size;
		}

		void PopBack() {
			if (m_size > 0) {
				--m_size;
				m_data[m_size].~_Ty();
			}
		}

		void ShrinkToFit() {
			if (m_size < m_capacity) {
				_Ty* new_data = m_size > 0 ? static_cast<_Ty*>(::operator new(m_size * sizeof(_Ty))) : nullptr;
				for (SizeType i = 0; i < m_size; ++i) {
					new (new_data + i) _Ty(Move(m_data[i]));
					m_data[i].~_Ty();
				}
				::operator delete(static_cast<void*>(m_data));
				m_data     = new_data;
				m_capacity = m_size;
			}
		}

		void Swap(Vector& other) {
			_SPARK Swap(m_data, other.m_data);
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_capacity, other.m_capacity);
		}

		Reference      Front() { return m_data[0]; }
		ConstReference Front() const { return m_data[0]; }

		Reference      Middle() { return m_data[m_size / 2]; }
		ConstReference Middle() const { return m_data[m_size / 2]; }

		Reference      Back() { return m_data[m_size - 1]; }
		ConstReference Back() const { return m_data[m_size - 1]; }

		Reference      Get(SizeType index) { return m_data[index]; }
		ConstReference Get(SizeType index) const { return m_data[index]; }

		Pointer      Data() { return m_data; }
		ConstPointer Data() const { return m_data; }

		void Reserve(SizeType new_capacity) {
			if (new_capacity > m_capacity) {
				_Ty* new_data = static_cast<_Ty*>(::operator new(new_capacity * sizeof(_Ty)));
				if (m_data) {
					for (SizeType i = 0; i < m_size; ++i) {
						new (new_data + i) _Ty(Move(m_data[i]));
						m_data[i].~_Ty();
					}
					::operator delete(static_cast<void*>(m_data));
				}
				m_data     = new_data;
				m_capacity = new_capacity;
			}
		}

		void SwapAndPop(SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			_SPARK Swap(m_data[index], m_data[m_size - 1]);
			PopBack();
		}

		void Resize(SizeType new_size) {
			if (new_size > m_capacity) {
				Reserve(new_size);
			}
			m_size = new_size;
		}

		template<typename... Args> void EmplaceBack(Args&&... args) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			new (&m_data[m_size]) _Ty(Forward<Args>(args)...);
			++m_size;
		}

		template<typename... Args> void EmplaceFront(Args&&... args) {
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			CopyBackwards(m_data, m_data + m_size, m_data + m_size + 1);
			new (&m_data[0]) _Ty(Forward<Args>(args)...);
			++m_size;
		}

		template<typename... Args> void Emplace(SizeType index, Args&&... args) {
			if (index > m_size) {
				LOG_FATAL("Index out of range");
			}
			if (m_size >= m_capacity) {
				Reserve(m_capacity == 0 ? 1 : m_capacity * 2);
			}
			if (!m_data) {
				m_data = static_cast<_Ty*>(::operator new(sizeof(_Ty)));
			}
			CopyBackwards(m_data + index, m_data + m_size, m_data + m_size + 1);
			new (&m_data[index]) _Ty(Forward<Args>(args)...);
			++m_size;
		}

		Iterator      Begin() { return Iterator(m_data); }
		Iterator      End() { return Iterator(m_data + m_size); }
		ConstIterator Begin() const { return Iterator(m_data); }
		ConstIterator End() const { return Iterator(m_data + m_size); }

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

	  private:
		void CopyBackwards(Pointer src_begin, Pointer src_end, Pointer dest_end) {
			while (src_end != src_begin) {
				--src_end;
				--dest_end;
				new (dest_end) _Ty(Move(*src_end));
				src_end->~_Ty();
			}
		}
		void Copy(Pointer src_begin, Pointer src_end, Pointer dest_begin) {
			while (src_begin != src_end) {
				new (dest_begin) _Ty(Move(*src_begin));
				src_begin->~_Ty();
				++src_begin;
				++dest_begin;
			}
		}

	  private:
		Pointer       m_data;
		AllocatorType m_allocator;
		SizeType        m_size;
		SizeType        m_capacity;
	};

	template<typename _Ty> void Swap(Vector<_Ty>& lhs, Vector<_Ty>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_VECTOR_HPP