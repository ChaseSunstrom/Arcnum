#ifndef SPARK_CLASSIC_UTIL_HPP
#define SPARK_CLASSIC_UTIL_HPP

#include <core/math/math.hpp>
#include <core/util/types.hpp>
#include "traits.hpp"

namespace Spark {
    /**
     * @brief Moves an object of type _Ty.
     * @param value The object to be moved.
     * @return An rvalue reference to the object.
     */
    template<typename _Ty>
    _Ty&& Move(_Ty& value) {
        return static_cast<_Ty&&>(value);
    }

    /**
     * @brief Swaps two objects of type _Ty.
     * @param a First object to swap.
     * @param b Second object to swap.
     */
    template<typename _Ty>
    void Swap(_Ty& a, _Ty& b) {
        _Ty&& tmp = Move(a);
        a = Move(b);
        b = Move(tmp);
    }

  
    /**
     * @brief Reverses the order of elements in the range [first, last).
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void Reverse(_Ty* first, _Ty* last) {
        while (first < last) {
            --last;
            Swap(first, last);
            ++first;
        }
    }

    /**
     * @brief Rotates the elements in the range [first, last), such that n_first becomes the first element.
     * @param first Pointer to the first element of the range.
     * @param n_first Pointer to the element that should become the first element after rotation.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void Rotate(_Ty* first, _Ty* n_first, _Ty* last) {
        Reverse(first, n_first);
        Reverse(n_first, last);
        Reverse(first, last);
    }

    /**
     * @brief Rotates the elements in the range [first, last) to the left by one position.
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void RotateLeft(_Ty* first, _Ty* last) {
        _Ty tmp = Move(*first);
        _Ty* i = first;
        _Ty* j = first + 1;
        while (j != last) {
            *i = Move(*j);
            ++i;
            ++j;
        }
        *i = Move(tmp);
    }

    /**
     * @brief Rotates the elements in the range [first, last) to the right by one position.
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void RotateRight(_Ty* first, _Ty* last) {
        _Ty tmp = Move(*(last - 1));
        _Ty* i = last - 1;
        _Ty* j = last - 2;
        while (j != first - 1) {
            *i = Move(*j);
            --i;
            --j;
        }
        *i = Move(tmp);
    }

    /**
     * @brief Constructs a max heap from elements in the range [first, last).
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void MakeHeap(_Ty* first, _Ty* last) {
        i32 n = static_cast<i32>(last - first);
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

    /**
     * @brief Pushes a new element onto a max heap.
     * @param first Pointer to the first element of the heap.
     * @param last Pointer to one past the last element of the heap (including the new element).
     */
    template<typename _Ty>
    void PushHeap(_Ty* first, _Ty* last) {
        i32 n = static_cast<i32>(last - first);
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

    /**
     * @brief Removes the top element from a max heap.
     * @param first Pointer to the first element of the heap.
     * @param last Pointer to one past the last element of the heap.
     */
    template<typename _Ty>
    void PopHeap(_Ty* first, _Ty* last) {
        Swap(first, last - 1);
        i32 n = static_cast<i32>(last - first - 1);
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

    /**
     * @brief Sorts elements in a max heap.
     * @param first Pointer to the first element of the heap.
     * @param last Pointer to one past the last element of the heap.
     */
    template<typename _Ty>
    void SortHeap(_Ty* first, _Ty* last) {
        while (last - first > 1) {
            PopHeap(first, last);
            --last;
        }
    }

    /**
     * @brief Sorts elements in a max heap using a buffer.
     * @param first Pointer to the first element of the heap.
     * @param last Pointer to one past the last element of the heap.
     * @param buffer Pointer to a buffer for temporary storage.
     */
    template<typename _Ty>
    void SortHeap(_Ty* first, _Ty* last, _Ty* buffer) {
        while (last - first > 1) {
            PopHeap(first, last);
            --last;
            Swap(first, last);
            PushHeap(first, last);
        }
    }

    /**
     * @brief Sorts elements using the bubble sort algorithm.
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void BubbleSort(_Ty* first, _Ty* last) {
        for (_Ty* i = first; i != last; ++i) {
            for (_Ty* j = last - 1; j != i; --j) {
                if (*j < *(j - 1)) {
                    Swap(j, j - 1);
                }
            }
        }
    }

    /**
     * @brief Sorts elements using the selection sort algorithm.
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     */
    template<typename _Ty>
    void SelectionSort(_Ty* first, _Ty* last) {
        for (_Ty* i = first; i != last; ++i) {
            _Ty* min = i;
            for (_Ty* j = i + 1; j != last; ++j) {
                if (*j < *min) {
                    min = j;
                }
            }
            Swap(i, min);
        }
    }

    /**
     * @brief Perfect forwarding for lvalue references.
     * @param t The object to forward.
     * @return An rvalue reference to the forwarded object.
     */
    template<typename _Ty>
    constexpr _Ty&& Forward(RemoveRefT<_Ty>& t) noexcept {
        return static_cast<_Ty&&>(t);
    }

    /**
     * @brief Perfect forwarding for rvalue references.
     * @param t The object to forward.
     * @return An rvalue reference to the forwarded object.
     */
    template<typename _Ty>
    constexpr _Ty&& Forward(RemoveRefT<_Ty>&& t) noexcept {
        return static_cast<_Ty&&>(t);
    }

    /**
     * @brief Merges two sorted ranges into a buffer.
     * @param first Pointer to the first element of the first range.
     * @param mid Pointer to one past the last element of the first range (and first element of the second range).
     * @param last Pointer to one past the last element of the second range.
     * @param buffer Pointer to a buffer for temporary storage.
     */
    template<typename _Ty>
    void Merge(_Ty* first, _Ty* mid, _Ty* last, _Ty* buffer) {
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
    template<typename _Ty>
    void InsertionSort(_Ty* first, _Ty* last) {
        for (_Ty* i = first + 1; i != last; ++i) {
            _Ty key = Move(*i);
            _Ty* j = i - 1;
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
    template<typename _Ty>
    _Ty* MedianOfThree(_Ty* a, _Ty* b, _Ty* c) {
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
    template<typename _Ty>
    void QuickSort(_Ty* first, _Ty* last, i32 max_depth) {
        while (last - first > 16) {
            if (max_depth == 0) {
                HeapSort(first, last);
                return;
            }

            --max_depth;
            _Ty* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
            Swap(pivot, last - 1);
            pivot = last - 1;

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
    template<typename _Ty>
    void QuickSort(_Ty* first, _Ty* last) {
        if (first == last)
            return;

        _Ty* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
        Swap(pivot, last - 1);
        pivot = last - 1;

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
    template<typename _Ty>
    _Ty* Partition(_Ty* first, _Ty* last) {
        _Ty* pivot = MedianOfThree(first, first + (last - first) / 2, last - 1);
        Swap(pivot, last - 1);
        pivot = last - 1;

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
    template<typename _Ty>
    void HeapSort(_Ty* first, _Ty* last) {
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
    template<typename _Ty>
    void IntroSortImpl(_Ty* first, _Ty* last, i32 max_depth) {
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
    template<typename _Ty>
    void Sort(_Ty* first, _Ty* last) {
        if (first == last)
            return;

        i32 max_depth = 2 * static_cast<i32>(_MATH Log2(last - first));
        IntroSortImpl(first, last, max_depth);
        InsertionSort(first, last);
    }

    /**
     * @brief Copies elements from one range to another.
     * @param first Pointer to the first element of the source range.
     * @param last Pointer to one past the last element of the source range.
     * @param d_first Pointer to the first element of the destination range.
     */
    template<typename _Ty>
    void Copy(const _Ty* first, const _Ty* last, _Ty* d_first) {
        while (first != last) {
            *d_first++ = *first++;
        }
    }

    /**
     * @brief Copies elements from one range to another (non-const overload).
     * @param first Pointer to the first element of the source range.
     * @param last Pointer to one past the last element of the source range.
     * @param d_first Pointer to the first element of the destination range.
     */
    template<typename _Ty>
    void Copy(_Ty* first, _Ty* last, _Ty* d_first) {
        Copy(static_cast<const _Ty*>(first), static_cast<const _Ty*>(last), d_first);
    }

    /**
     * @brief Copies elements from one range to another in reverse order.
     * @param first Pointer to the first element of the source range.
     * @param last Pointer to one past the last element of the source range.
     * @param d_last Pointer to one past the last element of the destination range.
     */
    template<typename _Ty>
    void CopyBackwards(const _Ty* first, const _Ty* last, _Ty* d_last) {
        while (first != last) {
            *--d_last = *--last;
        }
    }

    /**
     * @brief Copies elements from one range to another in reverse order (non-const overload).
     * @param first Pointer to the first element of the source range.
     * @param last Pointer to one past the last element of the source range.
     * @param d_last Pointer to one past the last element of the destination range.
     */
    template<typename _Ty>
    void CopyBackwards(_Ty* first, _Ty* last, _Ty* d_last) {
        CopyBackwards(static_cast<const _Ty*>(first), static_cast<const _Ty*>(last), d_last);
    }

    /**
     * @brief Moves elements from one range to another.
     * @param first Pointer to the first element of the source range.
     * @param last Pointer to one past the last element of the source range.
     * @param dest Pointer to the first element of the destination range.
     */
    template<typename _Ty>
    void Move(_Ty* first, _Ty* last, _Ty* dest) {
        while (first != last) {
            *dest = Move(*first);
            ++first;
            ++dest;
        }
    }

    /**
     * @brief Fills a range with a specified value.
     * @param first Pointer to the first element of the range.
     * @param last Pointer to one past the last element of the range.
     * @param value The value to fill the range with.
     */
    template<typename _Ty>
    void Fill(_Ty* first, _Ty* last, const _Ty& value) {
        while (first != last) {
            *first = value;
            ++first;
        }
    }

    /**
     * @brief Gets the address of an object.
     * @param value The object to get the address of.
     * @return Pointer to the object.
     */
    template<typename _Ty>
    _Ty* AddressOf(const _Ty& value) {
        return reinterpret_cast<_Ty*>(const_cast<char*>(&reinterpret_cast<const volatile char&>(value)));
    }

    /**
     * @brief Calculates the distance between two pointers.
     * @param first Pointer to the first element.
     * @param last Pointer to one past the last element.
     * @return The number of elements between first and last.
     */
    template<typename _Ty>
    ptrdiff_t Distance(_Ty* first, _Ty* last) {
        return last - first;
    }

    /**
     * @brief Calculates the distance between two const pointers.
     * @param first Pointer to the first element.
     * @param last Pointer to one past the last element.
     * @return The number of elements between first and last.
     */
    template<typename _Ty>
    ptrdiff_t Distance(const _Ty* first, const _Ty* last) {
        return last - first;
    }
} // namespace Spark

#endif