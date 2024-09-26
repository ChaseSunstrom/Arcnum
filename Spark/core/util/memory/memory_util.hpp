#ifndef SPARK_MEMORY_UTIL_HPP
#define SPARK_MEMORY_UTIL_HPP

#include <core/util/types.hpp>

namespace Spark {
	void*  MemSet(void* dest, i32 value, size_t count);
	void*  MemCpy(void* dest, const void* src, size_t count);
	void*  MemMove(void* dest, const void* src, size_t count);
	void*  MemCpyN(void* dest, const void* src, size_t count);
	void*  MemFind(const void* ptr, size_t count, i32 value);
	void*  MemFindN(const void* ptr, size_t count, const void* value, size_t value_count);
	void*  MemChr(const void* ptr, i32 value, size_t count);
	void*  MemReverse(void* ptr, size_t count);
	i32    MemCmp(const void* lhs, const void* rhs, size_t count);
	size_t MemSpn(const void* dest, size_t dest_count, const void* src, size_t src_count);
}

#endif