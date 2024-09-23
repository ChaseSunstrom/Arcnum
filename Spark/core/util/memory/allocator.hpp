#ifndef SPARK_ALLOCATOR_TRAITS_HPP
#define SPARK_ALLOCATOR_TRAITS_HPP

#include <core/util/classic/util.hpp>
#include <core/util/types.hpp>
#include <core/util/classic/traits.hpp>
#include <memory>

namespace Spark {
	template<typename _Ty> class Allocator;

	// Primary template for custom allocators
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

		static Pointer Allocate(Allocator& alloc, SizeType n) { return alloc.Allocate(n); }

		static void Deallocate(Allocator& alloc, Pointer p, SizeType n) { alloc.Deallocate(p, n); }

		template<typename _Uty, typename... Args> static void Construct(Allocator& alloc, _Uty* p, Args&&... args) { alloc.Construct(p, Forward<Args>(args)...); }

		template<typename _Uty> static void Destroy(Allocator& alloc, _Uty* p) { alloc.Destroy(p); }

		static SizeType MaxSize(const Allocator& alloc) { return alloc.MaxSize(); }

		static Allocator SelectOnContainerCopyConstruction(const Allocator& alloc) { return Allocator(alloc); }
	};

	// Partial specialization for standard allocators using SFINAE
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

		static Pointer Allocate(Allocator& alloc, SizeType n) { return std::allocator_traits<Allocator>::allocate(alloc, n); }

		static void Deallocate(Allocator& alloc, Pointer p, SizeType n) { std::allocator_traits<Allocator>::deallocate(alloc, p, n); }

		template<typename _Uty, typename... Args> static void Construct(Allocator& alloc, _Uty* p, Args&&... args) { std::allocator_traits<Allocator>::construct(alloc, p, std::forward<Args>(args)...); }

		template<typename _Uty> static void Destroy(Allocator& alloc, _Uty* p) { std::allocator_traits<Allocator>::destroy(alloc, p); }

		static SizeType MaxSize(const Allocator& alloc) { return std::allocator_traits<Allocator>::max_size(alloc); }

		static Allocator SelectOnContainerCopyConstruction(const Allocator& alloc) { return std::allocator_traits<Allocator>::select_on_container_copy_construction(alloc); }
	};

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

} // namespace Spark

#endif // SPARK_ALLOCATOR_TRAITS_HPP
