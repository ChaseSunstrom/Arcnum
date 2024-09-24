#ifndef SPARK_SHARED_PTR_HPP
#define SPARK_SHARED_PTR_HPP

#include <core/util/classic/traits.hpp>
#include <core/util/types.hpp>
#include "allocator.hpp"
#include <core/util/classic/util.hpp>

namespace Spark {

	template<typename T, typename Allocator = Allocator<T>> class SharedPtr {
	  public:
		using ValueType       = T;
		using Pointer         = T*;
		using Reference       = T&;
		using ConstPointer    = const T*;
		using ConstReference  = const T&;
		using SizeType        = usize;
		using DifferenceType  = ptrdiff;

		using AllocatorType   = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

	  private:
		struct ControlBlock {
			// Reference count
			i32 ref_count;
			// Managed object
			Pointer ptr;
			// Allocator
			AllocatorType allocator;

			ControlBlock(Pointer p, AllocatorType alloc)
				: ref_count(1)
				, ptr(p)
				, allocator(Move(alloc)) {}

			~ControlBlock() {
				if (ptr) {
					AllocatorTraits::Destroy(allocator, ptr);
					AllocatorTraits::Deallocate(allocator, ptr, 1);
					ptr = nullptr;
				}
			}
		};

		ControlBlock* m_control_block;

	  public:
		// Default constructor
		SharedPtr()
			: m_control_block(nullptr) {}

		// Constructor with nullptr
		SharedPtr(nullptr_t)
			: m_control_block(nullptr) {}

		// Constructor with raw pointer and allocator
		explicit SharedPtr(Pointer ptr, AllocatorType allocator = AllocatorType()) {
			if (ptr) {
				m_control_block = AllocateControlBlock(ptr, Move(allocator));
			} else {
				m_control_block = nullptr;
			}
		}

		// Copy constructor
		SharedPtr(const SharedPtr& other) {
			m_control_block = other.m_control_block;
			if (m_control_block) {
				++(m_control_block->ref_count);
			}
		}

		// Move constructor
		SharedPtr(SharedPtr&& other) {
			m_control_block       = other.m_control_block;
			other.m_control_block = nullptr;
		}

		// Destructor
		~SharedPtr() { Reset(); }

		// Copy assignment operator
		SharedPtr& operator=(const SharedPtr& other) {
			if (this != &other) {
				Reset();
				m_control_block = other.m_control_block;
				if (m_control_block) {
					++(m_control_block->ref_count);
				}
			}
			return *this;
		}

		// Move assignment operator
		SharedPtr& operator=(SharedPtr&& other) {
			if (this != &other) {
				Reset();
				m_control_block       = other.m_control_block;
				other.m_control_block = nullptr;
			}
			return *this;
		}

		// Reset method
		void Reset() {
			if (m_control_block) {
				--(m_control_block->ref_count);
				if (m_control_block->ref_count == 0) {
					AllocatorType alloc = Move(m_control_block->allocator);
					ControlBlock* cb    = m_control_block;
					m_control_block     = nullptr;

					// Destroy the control block
					AllocatorTraits::template Destroy<ControlBlock>(alloc, cb);
					AllocatorTraits::template Deallocate<ControlBlock>(alloc, cb, 1);
				} else {
					m_control_block = nullptr;
				}
			}
		}

		// Get the managed object pointer
		Pointer Get() const { return m_control_block ? m_control_block->ptr : nullptr; }

		// Dereference operator
		Reference operator*() const { return *(Get()); }

		// Member access operator
		Pointer operator->() const { return Get(); }

		// Use count
		i32 UseCount() const { return m_control_block ? m_control_block->ref_count : 0; }

		// Bool conversion
		explicit operator bool() const { return Get() != nullptr; }

	  private:
		// Helper function to allocate control block
		ControlBlock* AllocateControlBlock(Pointer ptr, AllocatorType allocator) {
			using ControlBlockAllocator = typename AllocatorTraits::template RebindAlloc<ControlBlock>;
			ControlBlockAllocator cb_allocator(allocator);
			ControlBlock*         cb = AllocatorTraits<ControlBlockAllocator>::Allocate(cb_allocator, 1);
			AllocatorTraits<ControlBlockAllocator>::Construct(cb_allocator, cb, ptr, Move(allocator));
			return cb;
		}

		// Copy constructor for casts
		template<typename U> SharedPtr(const SharedPtr<U, Allocator>& other, Pointer ptr) {
			m_control_block = other.m_control_block;
			if (m_control_block) {
				++(m_control_block->ref_count);
				m_control_block->ptr = ptr;
			}
		}
	};

	// MakeShared function
	template<typename T, typename Allocator = Allocator<T>, typename... Args> SharedPtr<T, Allocator> MakeShared(Allocator allocator, Args&&... args) {
		using AllocatorTraits = AllocatorTraits<Allocator>;
		// Allocate memory for object
		T* ptr                = AllocatorTraits::Allocate(allocator, 1);
		try {
			// Construct object
			AllocatorTraits::Construct(allocator, ptr, Forward<Args>(args)...);
			// Create SharedPtr
			return SharedPtr<T, Allocator>(ptr, Move(allocator));
		} catch (...) {
			AllocatorTraits::Deallocate(allocator, ptr, 1);
			throw;
		}
	}

	// Overload without allocator parameter
	template<typename T, typename... Args> SharedPtr<T> MakeShared(Args&&... args) {
		Allocator<T> allocator;
		return MakeShared<T, Allocator<T>>(allocator, Forward<Args>(args)...);
	}

	// Pointer casts
	template<typename To, typename From, typename Allocator> SharedPtr<To, Allocator> StaticPointerCast(const SharedPtr<From, Allocator>& from) {
		To* ptr = static_cast<To*>(from.Get());
		return ptr ? SharedPtr<To, Allocator>(from, ptr) : SharedPtr<To, Allocator>();
	}

	template<typename To, typename From, typename Allocator> SharedPtr<To, Allocator> DynamicPointerCast(const SharedPtr<From, Allocator>& from) {
		To* ptr = dynamic_cast<To*>(from.Get());
		return ptr ? SharedPtr<To, Allocator>(from, ptr) : SharedPtr<To, Allocator>();
	}

	template<typename To, typename From, typename Allocator> SharedPtr<To, Allocator> ReinterpretPointerCast(const SharedPtr<From, Allocator>& from) {
		To* ptr = reinterpret_cast<To*>(from.Get());
		return ptr ? SharedPtr<To, Allocator>(from, ptr) : SharedPtr<To, Allocator>();
	}

} // namespace Spark

#endif // SPARK_SHARED_PTR_HPP
