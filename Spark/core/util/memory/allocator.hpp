#ifndef SPARK_ALLOCATOR_TRAITS_HPP
#define SPARK_ALLOCATOR_TRAITS_HPP

#include <core/util/classic/util.hpp>
#include <core/util/types.hpp>
#include <core/util/classic/traits.hpp>
#include <core/util/container/iterator.hpp>
#include <memory>

namespace Spark {
	
	/**
	 * @brief Custom allocator class
	 * @tparam _Ty The type of elements to allocate
	 */
	template<typename _Ty> class Allocator {
	  public:
		using ValueType       = _Ty;
		using Pointer         = _Ty*;
		using ConstPointer    = const _Ty*;
		using Reference       = _Ty&;
		using ConstReference  = const _Ty&;
		using SizeType        = usize;
		using DifferenceType  = ptrdiff;

		using value_type      = ValueType;
		using pointer         = Pointer;
		using const_pointer   = ConstPointer;
		using reference       = Reference;
		using const_reference = ConstReference;
		using size_type       = SizeType;
		using difference_type = DifferenceType;

		template<typename _Uty> struct Rebind {
			using Other = Allocator<_Uty>;
		};

		Allocator() noexcept                 = default;
		Allocator(const Allocator&) noexcept = default;

		template<typename _Uty> Allocator(const Allocator<_Uty>&) noexcept {}

		~Allocator() = default;

		Pointer Allocate(SizeType n) { return static_cast<Pointer>(::operator new(n * sizeof(ValueType))); }

		void Deallocate(Pointer p, SizeType /*n*/) { ::operator delete(p); }

		template<typename _Uty, typename... Args> void Construct(_Uty* p, Args&&... args) { ::new (static_cast<void*>(p)) _Uty(Forward<Args>(args)...); }

		template<typename _Uty> void Destroy(_Uty* p) { p->~_Uty(); }

		SizeType MaxSize() const noexcept { return static_cast<SizeType>(-1) / sizeof(ValueType); }

		Pointer Address(Reference x) const noexcept { return AddressOf(x); }

		ConstPointer Address(ConstReference x) const noexcept { return AddressOf(x); }

		template<typename _Uty> bool operator==(const Allocator<_Uty>&) const noexcept { return true; }

		template<typename _Uty> bool operator!=(const Allocator<_Uty>&) const noexcept { return false; }

		// C++ 11 std library conformity methods
		template<typename _Uty> struct rebind {
			using other = Allocator<_Uty>;
		};

		pointer allocate(size_type n) { return Allocate(n); }

		void deallocate(pointer p, size_type n) { Deallocate(p, n); }

		template<typename _Uty, typename... Args> void construct(_Uty* p, Args&&... args) { Construct(p, Forward<Args>(args)...); }

		template<typename _Uty> void destroy(_Uty* p) { Destroy(p); }

		size_type max_size() const noexcept { return MaxSize(); }

		pointer address(reference x) const noexcept { return Address(x); }

		const_pointer address(const_reference x) const noexcept { return Address(x); }
	};


	/**
	 * @brief Primary template for custom allocators
	 * @tparam _Allocator The allocator type
	 */
	template<typename _Allocator, typename Enable = void> class AllocatorTraits {
	  public:
		using AllocatorType                    = _Allocator;
		using ValueType                        = typename AllocatorType::ValueType;
		using Pointer                          = typename AllocatorType::Pointer;
		using ConstPointer                     = typename AllocatorType::ConstPointer;
		using Reference                        = typename AllocatorType::Reference;
		using ConstReference                   = typename AllocatorType::ConstReference;
		using SizeType                         = typename AllocatorType::SizeType;
		using DifferenceType                   = typename AllocatorType::DifferenceType;

		template<typename _Uty> using RebindAlloc = typename AllocatorType::template Rebind<_Uty>::Other;

		static Pointer Allocate(AllocatorType& alloc, SizeType n) { return alloc.Allocate(n); }

		static void Deallocate(AllocatorType& alloc, Pointer p, SizeType n) { alloc.Deallocate(p, n); }

		template<typename _Uty, typename... Args>
		static void Construct(AllocatorType& alloc, _Uty* p, Args&&... args) {
			alloc.Construct(p, Forward<Args>(args)...);
		}

		template<typename _Uty>
		static void Destroy(AllocatorType& alloc, _Uty* p) {
			alloc.Destroy(p);
		}

		static SizeType MaxSize(const AllocatorType& alloc) noexcept { return alloc.MaxSize(); }

		static AllocatorType SelectOnContainerCopyConstruction(const AllocatorType& alloc) { return AllocatorType(alloc); }
	};

	/**
	 * @brief Partial specialization for standard allocators
	 * @tparam _Allocator The allocator type
	 */
	template<typename _Allocator>
	class AllocatorTraits<_Allocator, EnableIfT<!IsBaseOfV<_SPARK Allocator<typename _Allocator::ValueType>, _Allocator>>> {
	  public:
		using AllocatorType                    = _Allocator;
		using ValueType                        = typename AllocatorType::value_type;
		using Pointer                          = typename std::allocator_traits<AllocatorType>::pointer;
		using ConstPointer                     = typename std::allocator_traits<AllocatorType>::const_pointer;
		using Reference                        = ValueType&;
		using ConstReference                   = const ValueType&;
		using SizeType                         = typename AllocatorType::size_type;
		using DifferenceType                   = typename AllocatorType::difference_type;

		template<typename _Uty> using RebindAlloc = typename std::allocator_traits<AllocatorType>::template rebind_alloc<_Uty>;

		static Pointer Allocate(AllocatorType& alloc, SizeType n) { return std::allocator_traits<AllocatorType>::allocate(alloc, n); }

		static void Deallocate(AllocatorType& alloc, Pointer p, SizeType n) { std::allocator_traits<AllocatorType>::deallocate(alloc, p, n); }

		template<typename _Uty, typename... Args>
		static void Construct(AllocatorType& alloc, _Uty* p, Args&&... args) {
			std::allocator_traits<AllocatorType>::construct(alloc, p, Forward<Args>(args)...);
		}

		template<typename _Uty>
		static void Destroy(AllocatorType& alloc, _Uty* p) {
			std::allocator_traits<AllocatorType>::destroy(alloc, p);
		}

		static SizeType MaxSize(const AllocatorType& alloc) noexcept { return std::allocator_traits<AllocatorType>::max_size(alloc); }

		static AllocatorType SelectOnContainerCopyConstruction(const AllocatorType& alloc) {
			return std::allocator_traits<AllocatorType>::select_on_container_copy_construction(alloc);
		}
	};

	template<typename _Ty>
	typename Allocator<_Ty>::DifferenceType Distance(typename Allocator<_Ty>::Pointer first, typename Allocator<_Ty>::Pointer last) {
		typename Allocator<_Ty>::DifferenceType dist = 0;
		while (first != last) {
			++first;
			++dist;
		}
		return dist;
	}

	template<typename _Ty>
	typename Allocator<_Ty>::Pointer Next(typename Allocator<_Ty>::Pointer ptr, typename Allocator<_Ty>::DifferenceType n = 1) {
		return ptr + n;
	}

	template<typename _Ty>
	typename Allocator<_Ty>::Pointer Prev(typename Allocator<_Ty>::Pointer ptr, typename Allocator<_Ty>::DifferenceType n = 1) {
		return ptr - n;
	}

	
    /**
     * @brief Calculates the distance between two iterators.
     * @param first Iterator to the first element.
     * @param last Iterator to one past the last element.
     * @return The number of elements between first and last.
     */
    template<typename _Iterator>
    typename IteratorTraits<_Iterator>::DifferenceType Distance(_Iterator first, _Iterator last) {
        typename IteratorTraits<_Iterator>::DifferenceType result = 0;
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }


} // namespace Spark

#endif // SPARK_ALLOCATOR_TRAITS_HPP
