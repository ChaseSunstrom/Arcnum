#ifndef SPARK_ALLOCATOR_TRAITS_HPP
#define SPARK_ALLOCATOR_TRAITS_HPP

#include <core/util/classic/util.hpp>
#include <core/util/types.hpp>
#include <core/util/classic/traits.hpp>
#include <memory>

namespace Spark {
	template<typename _Ty> class Allocator;

	/**
	 * @brief Primary template for custom allocators
	 * @tparam Allocator The allocator type
	 * @tparam Enable SFINAE enabler
	 */
	template<typename Allocator, typename Enable = void> class AllocatorTraits {
	  public:
		using AllocatorType                    = Allocator;
		using ValueType                        = typename Allocator::ValueType;
		using Pointer                          = typename Allocator::Pointer;
		using ConstPointer                     = typename Allocator::ConstPointer;
		using Reference                        = typename Allocator::Reference;
		using ConstReference                   = typename Allocator::ConstReference;
		using SizeType                         = typename Allocator::SizeType;
		using DifferenceType                   = typename Allocator::DifferenceType;

		template<typename _Uty> using RebindAlloc = typename Allocator::template Rebind<_Uty>::Other;

		/**
		 * @brief Allocate memory
		 * @param alloc The allocator
		 * @param n The number of elements to allocate
		 * @return Pointer to the allocated memory
		 */
		static Pointer Allocate(Allocator& alloc, SizeType n) { return alloc.Allocate(n); }

		/**
		 * @brief Deallocate memory
		 * @param alloc The allocator
		 * @param p Pointer to the memory to deallocate
		 * @param n The number of elements to deallocate
		 */
		static void Deallocate(Allocator& alloc, Pointer p, SizeType n) { alloc.Deallocate(p, n); }

		/**
		 * @brief Construct an object
		 * @tparam _Uty The type of the object to construct
		 * @tparam Args The types of the arguments
		 * @param alloc The allocator
		 * @param p Pointer to the memory where the object should be constructed
		 * @param args The arguments to forward to the constructor
		 */
		template<typename _Uty, typename... Args> static void Construct(Allocator& alloc, _Uty* p, Args&&... args) { alloc.Construct(p, Forward<Args>(args)...); }

		/**
		 * @brief Destroy an object
		 * @tparam _Uty The type of the object to destroy
		 * @param alloc The allocator
		 * @param p Pointer to the object to destroy
		 */
		template<typename _Uty> static void Destroy(Allocator& alloc, _Uty* p) { alloc.Destroy(p); }

		/**
		 * @brief Get the maximum size that can be allocated
		 * @param alloc The allocator
		 * @return The maximum size
		 */
		static SizeType MaxSize(const Allocator& alloc) { return alloc.MaxSize(); }

		/**
		 * @brief Select on container copy construction
		 * @param alloc The allocator
		 * @return The selected allocator
		 */
		static Allocator SelectOnContainerCopyConstruction(const Allocator& alloc) { return Allocator(alloc); }
	};

	/**
	 * @brief Partial specialization for standard allocators using SFINAE
	 * @tparam Allocator The allocator type
	 */
	template<typename Allocator> class AllocatorTraits<Allocator, typename EnableIfT<!IsBaseOfV<_SPARK Allocator<typename Allocator::value_type>, Allocator>>> {
	  public:
		using AllocatorType                    = Allocator;
		using ValueType                        = typename Allocator::value_type;
		using Pointer                          = typename std::allocator_traits<Allocator>::pointer;
		using ConstPointer                     = typename std::allocator_traits<Allocator>::const_pointer;
		using Reference                        = ValueType&;
		using ConstReference                   = const ValueType&;
		using SizeType                         = typename Allocator::size_type;
		using DifferenceType                   = typename Allocator::difference_type;

		template<typename _Uty> using RebindAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<_Uty>;

		/**
		 * @brief Allocate memory
		 * @param alloc The allocator
		 * @param n The number of elements to allocate
		 * @return Pointer to the allocated memory
		 */
		static Pointer Allocate(Allocator& alloc, SizeType n) { return std::allocator_traits<Allocator>::allocate(alloc, n); }

		/**
		 * @brief Deallocate memory
		 * @param alloc The allocator
		 * @param p Pointer to the memory to deallocate
		 * @param n The number of elements to deallocate
		 */
		static void Deallocate(Allocator& alloc, Pointer p, SizeType n) { std::allocator_traits<Allocator>::deallocate(alloc, p, n); }

		/**
		 * @brief Construct an object
		 * @tparam _Uty The type of the object to construct
		 * @tparam Args The types of the arguments
		 * @param alloc The allocator
		 * @param p Pointer to the memory where the object should be constructed
		 * @param args The arguments to forward to the constructor
		 */
		template<typename _Uty, typename... Args> static void Construct(Allocator& alloc, _Uty* p, Args&&... args) { std::allocator_traits<Allocator>::construct(alloc, p, std::forward<Args>(args)...); }

		/**
		 * @brief Destroy an object
		 * @tparam _Uty The type of the object to destroy
		 * @param alloc The allocator
		 * @param p Pointer to the object to destroy
		 */
		template<typename _Uty> static void Destroy(Allocator& alloc, _Uty* p) { std::allocator_traits<Allocator>::destroy(alloc, p); }

		/**
		 * @brief Get the maximum size that can be allocated
		 * @param alloc The allocator
		 * @return The maximum size
		 */
		static SizeType MaxSize(const Allocator& alloc) { return std::allocator_traits<Allocator>::max_size(alloc); }

		/**
		 * @brief Select on container copy construction
		 * @param alloc The allocator
		 * @return The selected allocator
		 */
		static Allocator SelectOnContainerCopyConstruction(const Allocator& alloc) { return std::allocator_traits<Allocator>::select_on_container_copy_construction(alloc); }
	};

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
		using SizeType        = usize ;
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

		/**
		 * @brief Allocate memory
		 * @param n The number of elements to allocate
		 * @return Pointer to the allocated memory
		 */
		Pointer Allocate(SizeType n) { return static_cast<Pointer>(::operator new(n * sizeof(ValueType))); }

		/**
		 * @brief Deallocate memory
		 * @param p Pointer to the memory to deallocate
		 * @param n The number of elements to deallocate (unused)
		 */
		void Deallocate(Pointer p, SizeType /*n*/) { ::operator delete(p); }

		/**
		 * @brief Construct an object
		 * @tparam _Uty The type of the object to construct
		 * @tparam Args The types of the arguments
		 * @param p Pointer to the memory where the object should be constructed
		 * @param args The arguments to forward to the constructor
		 */
		template<typename _Uty, typename... Args> void Construct(_Uty* p, Args&&... args) { ::new (static_cast<void*>(p)) _Uty(Forward<Args>(args)...); }

		/**
		 * @brief Destroy an object
		 * @tparam _Uty The type of the object to destroy
		 * @param p Pointer to the object to destroy
		 */
		template<typename _Uty> void Destroy(_Uty* p) { p->~_Uty(); }

		/**
		 * @brief Get the maximum size that can be allocated
		 * @return The maximum size
		 */
		SizeType MaxSize() const noexcept { return static_cast<SizeType>(-1) / sizeof(ValueType); }

		/**
		 * @brief Get the address of an object
		 * @param x Reference to the object
		 * @return Pointer to the object
		 */
		Pointer Address(Reference x) const noexcept { return AddressOf(x); }

		/**
		 * @brief Get the address of a const object
		 * @param x Const reference to the object
		 * @return Const pointer to the object
		 */
		ConstPointer Address(ConstReference x) const noexcept { return AddressOf(x); }

		/**
		 * @brief Equality comparison
		 * @tparam _Uty The type of the other allocator
		 * @param other The other allocator
		 * @return Always true
		 */
		template<typename _Uty> bool operator==(const Allocator<_Uty>&) const noexcept { return true; }

		/**
		 * @brief Inequality comparison
		 * @tparam _Uty The type of the other allocator
		 * @param other The other allocator
		 * @return Always false
		 */
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
	 * @brief Calculate the distance between two pointers
	 * @tparam _Ty The type of elements pointed to
	 * @param first The first pointer
	 * @param last The last pointer
	 * @return The distance between the pointers
	 */
	template<typename _Ty>
	typename Allocator<_Ty>::DifferenceType Distance(typename Allocator<_Ty>::Pointer first, typename Allocator<_Ty>::Pointer last) {
		typename Allocator<_Ty>::DifferenceType dist = 0;
		while (first != last) {
			++first;
			++dist;
		}
		return dist;
	}

	/**
	 * @brief Advance a pointer by a given number of elements
	 * @tparam _Ty The type of elements pointed to
	 * @param ptr The pointer to advance
	 * @param n The number of elements to advance (default: 1)
	 * @return The advanced pointer
	 */
	template<typename _Ty>
	typename Allocator<_Ty>::Pointer Next(typename Allocator<_Ty>::Pointer ptr, typename Allocator<_Ty>::DifferenceType n = 1) {
		return ptr + n;
	}

	/**
	 * @brief Move a pointer backwards by a given number of elements
	 * @tparam _Ty The type of elements pointed to
	 * @param ptr The pointer to move backwards
	 * @param n The number of elements to move backwards (default: 1)
	 * @return The moved pointer
	 */
	template<typename _Ty>
	typename Allocator<_Ty>::Pointer Prev(typename Allocator<_Ty>::Pointer ptr, typename Allocator<_Ty>::DifferenceType n = 1) {
		return ptr - n;
	}

} // namespace Spark

#endif // SPARK_ALLOCATOR_TRAITS_HPP
