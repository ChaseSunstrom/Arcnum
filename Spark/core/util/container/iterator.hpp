#ifndef SPARK_ITERATOR_HPP
#define SPARK_ITERATOR_HPP

#include <core/util/types.hpp>
#include <core/util/classic/traits.hpp>

namespace Spark {

	struct InputIteratorTag {};

	struct OutputIteratorTag {};

	struct ForwardIteratorTag : public InputIteratorTag {};

	struct BidirectionalIteratorTag : public ForwardIteratorTag {};

	struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

	template<typename _Ty> class IteratorTraits {
	public:
		using IteratorCategory = typename _Ty::IteratorCategory;
		using ValueType = typename _Ty::ValueType;
		using DifferenceType = typename _Ty::DifferenceType;
		using PointerType = typename _Ty::PointerType;
		using ReferenceType = typename _Ty::ReferenceType;
	};

	// Specialization for pointer types
	template<typename _Ty>
	class IteratorTraits<_Ty*> {
	public:
		using IteratorCategory = RandomAccessIteratorTag;
		using ValueType = _Ty;
		using DifferenceType = ptrdiff_t;
		using PointerType = _Ty*;
		using ReferenceType = _Ty&;
	};

	// Specialization for const pointer types
	template<typename _Ty>
	class IteratorTraits<const _Ty*> {
	public:
		using IteratorCategory = RandomAccessIteratorTag;
		using ValueType = _Ty;
		using DifferenceType = ptrdiff_t;
		using PointerType = const _Ty*;
		using ReferenceType = const _Ty&;
	};

	template<typename _Category, typename _Ty, typename _Distance = ptrdiff_t,
		typename _Pointer = _Ty*, typename _Reference = _Ty&>
	class Iterator {
	public:
		using IteratorCategory = _Category;
		using ValueType = _Ty;
		using DifferenceType = _Distance;
		using PointerType = _Pointer;
		using ReferenceType = _Reference;
	};

	template<typename _Ty> using InputIterator         = Iterator<InputIteratorTag, _Ty>;

	template<typename _Ty> using OutputIterator        = Iterator<OutputIteratorTag, _Ty>;

	template<typename _Ty> using ForwardIterator       = Iterator<ForwardIteratorTag, _Ty>;

	template<typename _Ty> using BidirectionalIterator = Iterator<BidirectionalIteratorTag, _Ty>;

	template<typename _Ty> using RandomAccessIterator  = Iterator<RandomAccessIteratorTag, _Ty>;
	
	/**
	 * @brief Advances the iterator by the specified distance.
	 * @param it Iterator to advance.
	 * @param n Number of positions to advance the iterator.
	 */
	template<typename _Iterator>
	void Advance(_Iterator& it, typename IteratorTraits<_Iterator>::DifferenceType n) {
		using Category = typename IteratorTraits<_Iterator>::IteratorCategory;
		if constexpr (IsBaseOfV<RandomAccessIteratorTag, Category>) {
			it += n;
		} else {
			while (n > 0) {
				--n;
				++it;
			}
			while (n < 0) {
				++n;
				--it;
			}
		}
	}

	/**
	 * @brief Returns an iterator advanced by n positions.
	 * @param it Iterator to advance.
	 * @param n Number of positions to advance the iterator.
	 * @return An iterator advanced by n positions.
	 */
	template<typename _Iterator>
	_Iterator Next(_Iterator it, typename IteratorTraits<_Iterator>::DifferenceType n = 1) {
		Advance(it, n);
		return it;
	}

	/**
	 * @brief Returns an iterator moved back by n positions.
	 * @param it Iterator to move back.
	 * @param n Number of positions to move the iterator back.
	 * @return An iterator moved back by n positions.
	 */
	template<typename _Iterator>
	_Iterator Prev(_Iterator it, typename IteratorTraits<_Iterator>::DifferenceType n = 1) {
		Advance(it, -n);
		return it;
	}
}

#endif