#ifndef SPARK_ARRAY_HPP
#define SPARK_ARRAY_HPP

#include <array>
#include <initializer_list>
#include <core/util/classic/util.hpp>

namespace Spark {
	template<typename _Ty, i32 N> class Array {
	public:
		using ValueType      = _Ty;
		using SizeType       = i32;
		using Reference      = _Ty&;
		using ConstReference = const _Ty&;
		using Pointer        = _Ty*;
		using ConstPointer   = const _Ty*;
		using Iterator       = _Ty*;
		using ConstIterator  = const _Ty*;

		// Default constructor
		Array() {
			for (i32 i = 0; i < N; ++i) {
				new (m_data + i) _Ty();
			}
		}

		// Fill constructor
		explicit Array(const _Ty& value) { Fill(value); }

		Array(const _Ty& value, i32 count) { for (i32 i = 0; i < count; ++i) { m_data[i] = value; } }

		Array(const std::array<_Ty, N>& other) { for (i32 i = 0; i < N; ++i) { m_data[i] = other[i]; } }

		Array(const std::initializer_list<_Ty>& init) { Copy(init.begin(), init.end(), m_data); }

		Array(const Array& other) { Copy(other.Begin(), other.End(), m_data); }

		~Array() {
			for (i32 i = 0; i < N; ++i) {
				m_data[i].~_Ty();
			}
		}

		Array& operator=(const Array& other) {
			if (this != &other) { Copy(other.begin(), other.end(), m_data); }
			return *this;
		}

		Reference operator[](SizeType index) { return m_data[index]; }

		ConstReference operator[](SizeType index) const { return m_data[index]; }

		Reference At(SizeType index) {
			if (index >= N) { LOG_FATAL("Array index out of bounds"); }
			return m_data[index];
		}

		ConstReference At(SizeType index) const {
			if (index >= N) { LOG_FATAL("Array index out of bounds"); }
			return m_data[index];
		}

		friend std::ostream& operator<<(std::ostream& os, const Array& array) {
			os << "[";
			for (i32 i = 0; i < N; ++i) {
				os << array.m_data[i];
				if (i < N - 1) { os << ", "; }
				if (i == 10) { os << "..."; break; }
			}
			os << "]";
			return os;
		}
		
		Reference Front() { return m_data[0]; }

		ConstReference Front() const { return m_data[0]; }

		Reference Back() { return m_data[N - 1]; }

		ConstReference Back() const { return m_data[N - 1]; }

		Pointer Data() { return m_data; }

		ConstPointer Data() const { return m_data; }

		Iterator Begin() { return m_data; }

		ConstIterator Begin() const { return m_data; }

		Iterator End() { return m_data + N; }

		ConstIterator End() const { return m_data + N; }

		Iterator begin() { return Begin(); }

		ConstIterator begin() const { return Begin(); }

		Iterator end() { return End(); }

		ConstIterator end() const { return End(); }

		constexpr bool Empty() const { return N == 0; }

		constexpr SizeType Size() const { return N; }

		constexpr SizeType MaxSize() const { return N; }

		void Fill(const _Ty& value) { for (SizeType i = 0; i < N; ++i) { m_data[i] = value; } }

		void Swap(Array& other) { for (SizeType i = 0; i < N; ++i) { _SPARKSwap(m_data[i], other.m_data[i]); } }

		bool operator==(const Array& other) const {
			for (i32 i = 0; i < N; ++i) { if (m_data[i] != other.m_data[i]) { return false; } }

			return true;
		}

		bool operator!=(const Array& other) const { return !(*this == other); }

	private:
		alignas(_Ty) u8 m_data[N * sizeof(_Ty)];
	};

	template<typename _Ty, i32 N> void Swap(Array<_Ty, N>& lhs, Array<_Ty, N>& rhs) { lhs.Swap(rhs); }
}

#endif // SPARK_ARRAY_HPP