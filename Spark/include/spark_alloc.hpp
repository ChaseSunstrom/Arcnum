#ifndef SPARK_ALLOC_HPP
#define SPARK_ALLOC_HPP


#if defined(_MSC_VER)
#include <malloc.h> // For _aligned_malloc and _aligned_free
#endif

#include <cstdlib>
#include "spark_types.hpp"

namespace spark
{
    // Cross-platform aligned memory allocation
    inline opaque AlignedAlloc(usize alignment, usize size)
    {
#if defined(_MSC_VER)
        // Use _aligned_malloc on Windows
        return _aligned_malloc(size, alignment);
#elif defined(__APPLE__) || defined(__unix__)
        // Use posix_memalign on POSIX systems
        opaque ptr = nullptr;
        if (posix_memalign(&ptr, alignment, size) != 0)
        {
            return nullptr;
        }
        return ptr;
#else
        // Fallback for standard C++17 std::aligned_alloc
        return std::aligned_alloc(alignment, size);
#endif
    }

    // Cross-platform aligned memory deallocation
    inline void AlignedFree(opaque ptr)
    {
#if defined(_MSC_VER)
        _aligned_free(ptr); // Use _aligned_free on Windows
#else
        std::free(ptr); // Use standard free on POSIX systems
#endif
    }
}


#endif