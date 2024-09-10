#ifndef SPARK_UTIL_HPP
#define SPARK_UTIL_HPP

#include <core/math/math.hpp>
#include <core/util/types.hpp>

namespace Spark {
	template<typename T> T&& Move(T& value) { return static_cast<T&&>(value); }

	template<typename T> void Swap(T& a, T& b) {
		T tmp = Move(a);
		a     = Move(b);
		b     = Move(tmp);
	}

	template<typename T> void Swap(T* a, T* b) {
		T tmp = Move(*a);
		*a    = Move(*b);
		*b    = Move(tmp);
	}

	template<typename T> void Reverse(T* first, T* last) {
		while (first < last) {
			--last;
			Swap(first, last);
			++first;
		}
	}

	template<typename T> void Rotate(T* first, T* n_first, T* last) {
		Reverse(first, n_first);
		Reverse(n_first, last);
		Reverse(first, last);
	}

	template<typename T> void RotateLeft(T* first, T* last) {
		T  tmp = Move(*first);
		T* i   = first;
		T* j   = first + 1;
		while (j != last) {
			*i = Move(*j);
			++i;
			++j;
		}
		*i = Move(tmp);
	}

	template<typename T> void RotateRight(T* first, T* last) {
		T  tmp = Move(*(last - 1));
		T* i   = last - 1;
		T* j   = last - 2;
		while (j != first - 1) {
			*i = Move(*j);
			--i;
			--j;
		}
		*i = Move(tmp);
	}

	template<typename T> void MakeHeap(T* first, T* last) {
		i32 n = last - first;
		for (i32 i = n / 2 - 1; i >= 0; --i) {
			i32 j = i;
			while (true) {
				i32 k = 2 * j + 1;
				if (k >= n)
					break;
				if (k + 1 < n && first[k] < first[k + 1])
					++k;
				if (first[j] < first[k]) {
					Swap(first + j, first + k);
					j = k;
				} else {
					break;
				}
			}
		}
	}

	template<typename T> void PushHeap(T* first, T* last) {
		i32 n = last - first;
		i32 i = n - 1;
		while (i > 0) {
			i32 j = (i - 1) / 2;
			if (first[j] < first[i]) {
				Swap(first + j, first + i);
				i = j;
			} else {
				break;
			}
		}
	}

	template<typename T> void PopHeap(T* first, T* last) {
		Swap(first, last - 1);
		i32 n = last - first - 1;
		i32 i = 0;
		while (true) {
			i32 j = 2 * i + 1;
			if (j >= n)
				break;
			if (j + 1 < n && first[j] < first[j + 1])
				++j;
			if (first[i] < first[j]) {
				Swap(first + i, first + j);
				i = j;
			} else {
				break;
			}
		}
	}

	template<typename T> void SortHeap(T* first, T* last) {
		while (last - first > 1) {
			PopHeap(first, last);
			--last;
		}
	}

	template<typename T> void SortHeap(T* first, T* last, T* buffer) {
		while (last - first > 1) {
			PopHeap(first, last);
			--last;
			Swap(first, last);
			PushHeap(first, last);
		}
	}

	template<typename T> void BubbleSort(T* first, T* last) {
		for (T* i = first; i != last; ++i) {
			for (T* j = last - 1; j != i; --j) {
				if (*j < *(j - 1)) {
					Swap(j, j - 1);
				}
			}
		}
	}

	template<typename T> void SelectionSort(T* first, T* last) {
		for (T* i = first; i != last; ++i) {
			T* min = i;
			for (T* j = i + 1; j != last; ++j) {
				if (*j < *min) {
					min = j;
				}
			}
			Swap(i, min);
		}
	}

	template<typename T> constexpr T&& Forward(std::remove_reference_t<T>& t) noexcept { return static_cast<T&&>(t); }

	template<typename T> constexpr T&& Forward(std::remove_reference_t<T>&& t) noexcept { return static_cast<T&&>(t); }

	template<typename T> void Merge(T* first, T* mid, T* last, T* buffer) {
		T* i = first;
		T* j = mid;
		T* k = buffer;
		while (i != mid && j != last) {
			if (*i < *j) {
				*k = Move(*i);
				++i;
			} else {
				*k = Move(*j);
				++j;
			}
			++k;
		}
		while (i != mid) {
			*k = Move(*i);
			++i;
			++k;
		}
		while (j != last) {
			*k = Move(*j);
			++j;
			++k;
		}
		Copy(buffer, buffer + (last - first), first);
	}

	template<typename T> void InsertionSort(T* first, T* last) {
		for (T* i = first + 1; i != last; ++i) {
			T  key = Move(*i);
			T* j   = i - 1;
			while (j >= first && *j > key) {
				*(j + 1) = Move(*j);
				--j;
			}
			*(j + 1) = Move(key);
		}
	}

	template<typename T> T* MedianOfThree(T* a, T* b, T* c) {
		if (*a < *b) {
			if (*b < *c)
				return b;
			if (*a < *c)
				return c;
			return a;
		}
		if (*a < *c)
			return a;
		if (*b < *c)
			return c;
		return b;
	}

	template<typename T> void QuickSort(T* first, T* last, i32 max_depth) {
		while (last - first > 16) {
			if (max_depth == 0) {
				HeapSort(first, last);
				return;
			}

			--max_depth;
			T* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
			Swap(pivot, last - 1);
			pivot = last - 1;

			T* i  = first - 1;
			for (T* j = first; j != last - 1; ++j) {
				if (*j <= *pivot) {
					++i;
					Swap(i, j);
				}
			}
			Swap(i + 1, last - 1);

			QuickSort(first, i, max_depth);
			first = i + 2;
		}
		InsertionSort(first, last);
	}

	template<typename T> void QuickSort(T* first, T* last) {
		if (first == last)
			return;

		T* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
		Swap(pivot, last - 1);
		pivot = last - 1;

		T* i  = first - 1;
		for (T* j = first; j != last - 1; ++j) {
			if (*j <= *pivot) {
				++i;
				Swap(i, j);
			}
		}
		Swap(i + 1, last - 1);

		QuickSort(first, i);
		QuickSort(i + 2, last);
	}

	template<typename T> T* Partition(T* first, T* last) {
		T* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
		Swap(pivot, last - 1);
		pivot = last - 1;

		T* i  = first - 1;
		for (T* j = first; j != last - 1; ++j) {
			if (*j <= *pivot) {
				++i;
				Swap(i, j);
			}
		}
		Swap(i + 1, last - 1);
		return i + 1;
	}

	template<typename T> void HeapSort(T* first, T* last) {
		MakeHeap(first, last);
		for (T* i = last; i != first; --i) {
			PopHeap(first, i);
		}
	}

	template<typename T> void IntroSortImpl(T* first, T* last, i32 max_depth) {
		while (last - first > 16) {
			if (max_depth == 0) {
				HeapSort(first, last);
				return;
			}

			--max_depth;
			T* pivot = Partition(first, last);
			IntroSortImpl(pivot + 1, last, max_depth);
			last = pivot;
		}
	}

	template<typename T> void Sort(T* first, T* last) {
		if (first == last)
			return;

		i32 max_depth = 2 * static_cast<i32>(Math::Log2(last - first));
		IntroSortImpl(first, last, max_depth);
		InsertionSort(first, last);
	}

	template<typename T> void Copy(const T* first, const T* last, T* d_first) {
		while (first != last) {
			*d_first++ = *first++;
		}
	}

	// Overload for non-const source and destination
	template<typename T> void Copy(T* first, T* last, T* d_first) { Copy(static_cast<const T*>(first), static_cast<const T*>(last), d_first); }

	template<typename T> void CopyBackwards(const T* first, const T* last, T* d_last) {
		while (first != last) {
			*--d_last = *--last;
		}
	}

	template<typename T> void CopyBackwards(T* first, T* last, T* d_last) { CopyBackwards(static_cast<const T*>(first), static_cast<const T*>(last), d_last); }

	template<typename T> void Move(T* first, T* last, T* dest) {
		while (first != last) {
			*dest = Move(*first);
			++first;
			++dest;
		}
	}

	template<typename T> void Fill(T* first, T* last, const T& value) {
		while (first != last) {
			*first = value;
			++first;
		}
	}
} // namespace Spark

#endif