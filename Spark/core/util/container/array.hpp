#ifndef SPARK_ARRAY_HPP
#define SPARK_ARRAY_HPP

#include <array>
#include <core/util/classic/util.hpp>
#include <initializer_list>

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
				new (reinterpret_cast<_Ty*>(m_data) + i) _Ty();
			}
		}

		// Fill constructor
		explicit Array(const _Ty& value) { Fill(value); }

		Array(const _Ty& value, i32 count) {
			for (i32 i = 0; i < Min(count, N); ++i) {
				reinterpret_cast<_Ty*>(m_data)[i] = value;
			}
		}

		Array(const std::array<_Ty, N>& other) {
			for (i32 i = 0; i < N; ++i) {
				reinterpret_cast<_Ty*>(m_data)[i] = other[i];
			}
		}

		Array(const std::initializer_list<_Ty>& init) {
			auto it = init.begin();
			for (i32 i = 0; i < N && it != init.end(); ++i, ++it) {
				reinterpret_cast<_Ty*>(m_data)[i] = *it;
			}
		}

		Array(const Array& other) {
			for (i32 i = 0; i < N; ++i) {
				reinterpret_cast<_Ty*>(m_data)[i] = other[i];
			}
		}

		~Array() {
			for (i32 i = 0; i < N; ++i) {
				reinterpret_cast<_Ty*>(m_data)[i].~_Ty();
			}
		}

		Array& operator=(const Array& other) {
			if (this != &other) {
				for (i32 i = 0; i < N; ++i) {
					reinterpret_cast<_Ty*>(m_data)[i] = other[i];
				}
			}
			return *this;
		}

		Reference operator[](SizeType index) { return reinterpret_cast<_Ty*>(m_data)[index]; }

		ConstReference operator[](SizeType index) const { return reinterpret_cast<const _Ty*>(m_data)[index]; }

		Reference At(SizeType index) {
			if (index >= N) {
				LOG_FATAL("Array index out of bounds");
			}
			return reinterpret_cast<_Ty*>(m_data)[index];
		}

		ConstReference At(SizeType index) const {
			if (index >= N) {
				LOG_FATAL("Array index out of bounds");
			}
			return reinterpret_cast<const _Ty*>(m_data)[index];
		}

		friend std::ostream& operator<<(std::ostream& os, const Array& array) {
			os << "[";
			for (i32 i = 0; i < N; ++i) {
				os << array[i];
				if (i < N - 1) {
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

		Reference      Front() { return reinterpret_cast<_Ty*>(m_data)[0]; }
		ConstReference Front() const { return reinterpret_cast<const _Ty*>(m_data)[0]; }
		Reference      Back() { return reinterpret_cast<_Ty*>(m_data)[N - 1]; }
		ConstReference Back() const { return reinterpret_cast<const _Ty*>(m_data)[N - 1]; }

		Pointer      Data() { return reinterpret_cast<_Ty*>(m_data); }
		ConstPointer Data() const { return reinterpret_cast<const _Ty*>(m_data); }

		Iterator      Begin() { return reinterpret_cast<_Ty*>(m_data); }
		ConstIterator Begin() const { return reinterpret_cast<const _Ty*>(m_data); }
		Iterator      End() { return reinterpret_cast<_Ty*>(m_data) + N; }
		ConstIterator End() const { return reinterpret_cast<const _Ty*>(m_data) + N; }

		Iterator      begin() { return Begin(); }
		ConstIterator begin() const { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator end() const { return End(); }

		constexpr bool     Empty() const { return N == 0; }
		constexpr SizeType Size() const { return N; }
		constexpr SizeType MaxSize() const { return N; }

		void Fill(const _Ty& value) {
			for (SizeType i = 0; i < N; ++i) {
				reinterpret_cast<_Ty*>(m_data)[i] = value;
			}
		}

		void Swap(Array& other) {
			for (SizeType i = 0; i < N; ++i) {
				Spark::Swap(reinterpret_cast<_Ty*>(m_data)[i], reinterpret_cast<_Ty*>(other.m_data)[i]);
			}
		}

		bool operator==(const Array& other) const {
			for (i32 i = 0; i < N; ++i) {
				if (reinterpret_cast<const _Ty*>(m_data)[i] != other[i]) {
					return false;
				}
			}
			return true;
		}

		bool operator!=(const Array& other) const { return !(*this == other); }

	  private:
		alignas(_Ty) u8 m_data[N * sizeof(_Ty)];
	};

	template<typename _Ty, i32 N> void Swap(Array<_Ty, N>& lhs, Array<_Ty, N>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_ARRAY_HPP