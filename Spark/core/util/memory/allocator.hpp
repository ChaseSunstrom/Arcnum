#ifndef SPARK_ALLOCATOR_TRAITS_HPP
#define SPARK_ALLOCATOR_TRAITS_HPP

#include <memory>  
#include <core/util/classic/util.hpp>

namespace Spark {
	template<typename T> class Allocator;

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

		template<typename U> using RebindAlloc = typename Allocator::template Rebind<U>::Other;

		static Pointer Allocate(Allocator& alloc, SizeType n) { return alloc.Allocate(n); }

		static void Deallocate(Allocator& alloc, Pointer p, SizeType n) { alloc.Deallocate(p, n); }

		template<typename U, typename... Args> static void Construct(Allocator& alloc, U* p, Args&&... args) { alloc.Construct(p, Forward<Args>(args)...); }

		template<typename U> static void Destroy(Allocator& alloc, U* p) { alloc.Destroy(p); }

		static SizeType MaxSize(const Allocator& alloc) { return alloc.MaxSize(); }

		static Allocator SelectOnContainerCopyConstruction(const Allocator& alloc) { return Allocator(alloc); }
	};

	// Partial specialization for standard allocators using SFINAE
	template<typename Allocator> class AllocatorTraits<Allocator, typename std::enable_if<!std::is_base_of<Spark::Allocator<typename Allocator::value_type>, Allocator>::value>::type> {
	  public:
		using AllocatorType                    = Allocator;
		using ValueType                        = typename Allocator::value_type;
		using Pointer                          = typename std::allocator_traits<Allocator>::pointer;
		using ConstPointer                     = typename std::allocator_traits<Allocator>::const_pointer;
		using Reference                        = ValueType&;
		using ConstReference                   = const ValueType&;
		using SizeType                         = typename Allocator::size_type;
		using DifferenceType                   = typename Allocator::difference_type;

		template<typename U> using RebindAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<U>;

		static Pointer Allocate(Allocator& alloc, SizeType n) { return std::allocator_traits<Allocator>::allocate(alloc, n); }

		static void Deallocate(Allocator& alloc, Pointer p, SizeType n) { std::allocator_traits<Allocator>::deallocate(alloc, p, n); }

		template<typename U, typename... Args> static void Construct(Allocator& alloc, U* p, Args&&... args) { std::allocator_traits<Allocator>::construct(alloc, p, std::forward<Args>(args)...); }

		template<typename U> static void Destroy(Allocator& alloc, U* p) { std::allocator_traits<Allocator>::destroy(alloc, p); }

		static SizeType MaxSize(const Allocator& alloc) { return std::allocator_traits<Allocator>::max_size(alloc); }

		static Allocator SelectOnContainerCopyConstruction(const Allocator& alloc) { return std::allocator_traits<Allocator>::select_on_container_copy_construction(alloc); }
	};

	 template<typename T> class Allocator {
	  public:
		using ValueType      = T;
		using Pointer        = T*;
		using ConstPointer   = const T*;
		using Reference      = T&;
		using ConstReference = const T&;
		using SizeType       = std::size_t;
		using DifferenceType = std::ptrdiff_t;

		template<typename U> struct Rebind {
			using Other = Allocator<U>;
		};

		Allocator() noexcept                 = default;
		Allocator(const Allocator&) noexcept = default;

		template<typename U> Allocator(const Allocator<U>&) noexcept {}

		~Allocator() = default;

		Pointer Allocate(SizeType n) { return static_cast<Pointer>(::operator new(n * sizeof(ValueType))); }

		void Deallocate(Pointer p, SizeType /*n*/) { ::operator delete(p); }

		template<typename U, typename... Args> void Construct(U* p, Args&&... args) { ::new (static_cast<void*>(p)) U(Forward<Args>(args)...); }

		template<typename U> void Destroy(U* p) { p->~U(); }

		SizeType MaxSize() const noexcept { return static_cast<SizeType>(-1) / sizeof(ValueType); }

		Pointer Address(Reference x) const noexcept { return AddressOf(x); }

		ConstPointer Address(ConstReference x) const noexcept { return AddressOf(x); }

		template<typename U> bool operator==(const Allocator<U>&) const noexcept { return true; }

		template<typename U> bool operator!=(const Allocator<U>&) const noexcept { return false; }
	};

} // namespace Spark

#endif // SPARK_ALLOCATOR_TRAITS_HPP
