#ifndef SPARK_SORT_HPP
#define SPARK_SORT_HPP

#include <core/math/math.hpp>
#include "util.hpp"

namespace Spark {

	/**
	 * @brief Merges two sorted ranges into a buffer.
	 * @param first Pointer to the first element of the first range.
	 * @param mid Pointer to one past the last element of the first range (and first element of the second range).
	 * @param last Pointer to one past the last element of the second range.
	 * @param buffer Pointer to a buffer for temporary storage.
	 */
	template<typename _Ty> void Merge(_Ty* first, _Ty* mid, _Ty* last, _Ty* buffer) {
		_Ty* i = first;
		_Ty* j = mid;
		_Ty* k = buffer;
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

	/**
	 * @brief Sorts elements using the insertion sort algorithm.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 */
	template<typename _Ty> void InsertionSort(_Ty* first, _Ty* last) {
		for (_Ty* i = first + 1; i != last; ++i) {
			_Ty  key = Move(*i);
			_Ty* j   = i - 1;
			while (j >= first && *j > key) {
				*(j + 1) = Move(*j);
				--j;
			}
			*(j + 1) = Move(key);
		}
	}

	/**
	 * @brief Finds the median of three elements.
	 * @param a Pointer to the first element.
	 * @param b Pointer to the second element.
	 * @param c Pointer to the third element.
	 * @return Pointer to the median element.
	 */
	template<typename _Ty> _Ty* MedianOfThree(_Ty* a, _Ty* b, _Ty* c) {
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

	/**
	 * @brief Sorts elements using the quicksort algorithm with a maximum recursion depth.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 * @param max_depth Maximum recursion depth before switching to heapsort.
	 */
	template<typename _Ty> void QuickSort(_Ty* first, _Ty* last, i32 max_depth) {
		while (last - first > 16) {
			if (max_depth == 0) {
				HeapSort(first, last);
				return;
			}

			--max_depth;
			_Ty* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
			Swap(pivot, last - 1);
			pivot  = last - 1;

			_Ty* i = first - 1;
			for (_Ty* j = first; j != last - 1; ++j) {
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

	/**
	 * @brief Sorts elements using the quicksort algorithm.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 */
	template<typename _Ty> void QuickSort(_Ty* first, _Ty* last) {
		if (first == last)
			return;

		_Ty* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
		Swap(pivot, last - 1);
		pivot  = last - 1;

		_Ty* i = first - 1;
		for (_Ty* j = first; j != last - 1; ++j) {
			if (*j <= *pivot) {
				++i;
				Swap(i, j);
			}
		}
		Swap(i + 1, last - 1);

		QuickSort(first, i);
		QuickSort(i + 2, last);
	}

	/**
	 * @brief Partitions elements around a pivot for quicksort.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 * @return Pointer to the pivot element after partitioning.
	 */
	template<typename _Ty> _Ty* Partition(_Ty* first, _Ty* last) {
		_Ty* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
		Swap(pivot, last - 1);
		pivot  = last - 1;

		_Ty* i = first - 1;
		for (_Ty* j = first; j != last - 1; ++j) {
			if (*j <= *pivot) {
				++i;
				Swap(i, j);
			}
		}
		Swap(i + 1, last - 1);
		return i + 1;
	}

	/**
	 * @brief Sorts elements using the heapsort algorithm.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 */
	template<typename _Ty> void HeapSort(_Ty* first, _Ty* last) {
		MakeHeap(first, last);
		for (_Ty* i = last; i != first; --i) {
			PopHeap(first, i);
		}
	}

	/**
	 * @brief Implementation of introsort algorithm.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 * @param max_depth Maximum recursion depth before switching to heapsort.
	 */
	template<typename _Ty> void IntroSortImpl(_Ty* first, _Ty* last, i32 max_depth) {
		while (last - first > 16) {
			if (max_depth == 0) {
				HeapSort(first, last);
				return;
			}

			--max_depth;
			_Ty* pivot = Partition(first, last);
			IntroSortImpl(pivot + 1, last, max_depth);
			last = pivot;
		}
	}

	/**
	 * @brief Sorts elements using the introsort algorithm.
	 * @param first Pointer to the first element of the range.
	 * @param last Pointer to one past the last element of the range.
	 */
	template<typename _Ty> void Sort(_Ty* first, _Ty* last) {
		if (first == last)
			return;

		i32 max_depth = 2 * static_cast<i32>(_MATH Log2(last - first));
		IntroSortImpl(first, last, max_depth);
		InsertionSort(first, last);
	}
}


#endif