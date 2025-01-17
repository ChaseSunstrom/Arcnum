#ifndef SPARK_BUMP_ALLOCATOR_HPP
#define SPARK_BUMP_ALLOCATOR_HPP

#include "spark_pch.hpp"

namespace spark {
    class BumpAllocator {
    public:
        explicit BumpAllocator(std::size_t total_size)
            : m_total_size(total_size), m_offset(0)
        {
            m_buffer = static_cast<std::byte*>(std::malloc(total_size));
            if (!m_buffer)
            {
                throw std::bad_alloc();
            }
        }

        ~BumpAllocator()
        {
            std::free(m_buffer);
        }

        opaque Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t))
        {
            std::size_t aligned_offset = (m_offset + alignment - 1) & ~(alignment - 1);

            if (aligned_offset + size > m_total_size)
            {
                throw std::bad_alloc();
            }

            opaque ptr = m_buffer + aligned_offset;
            m_offset = aligned_offset + size;

            return ptr;
        }

        void Reset()
        {
            m_offset = 0;
        }

    private:
        std::byte* m_buffer;
        std::size_t m_total_size;
        std::size_t m_offset;
    };

    template <typename T>
    class HighPerformanceAllocator {
    public:
        using value_type = T;

        HighPerformanceAllocator(BumpAllocator& allocator)
            : m_allocator(allocator) {}

        template <typename U>
        HighPerformanceAllocator(const HighPerformanceAllocator<U>& other)
            : m_allocator(other.m_allocator) {}

        T* allocate(std::size_t n)
        {
            return static_cast<T*>(m_allocator.Allocate(n * sizeof(T), alignof(T)));
        }

        void deallocate(T*, std::size_t) noexcept
        {
            // No-op, as the entire buffer is managed as a whole.
        }

    private:
        BumpAllocator& m_allocator;

        template <typename U>
        friend class HighPerformanceAllocator;
    };
}

#endif