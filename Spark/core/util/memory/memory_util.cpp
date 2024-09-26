#include "memory_util.hpp"

namespace Spark {
	void* MemSet(void* dest, i32 value, size_t count) {
		u8* ptr = static_cast<u8*>(dest);
		while (count--) {
			*ptr++ = static_cast<u8>(value);
		}
		return dest;
	}

	void* MemCpy(void* dest, const void* src, size_t count) {
		u8*       d = static_cast<u8*>(dest);
		const u8* s = static_cast<const u8*>(src);
		while (count--) {
			*d++ = *s++;
		}
		return dest;
	}

	void* MemMove(void* dest, const void* src, size_t count) {
		u8*       d = static_cast<u8*>(dest);
		const u8* s = static_cast<const u8*>(src);
		if (d < s) {
			while (count--) {
				*d++ = *s++;
			}
		} else {
			d += count;
			s += count;
			while (count--) {
				*--d = *--s;
			}
		}
		return dest;
	}

	void* MemCpyN(void* dest, const void* src, size_t count) {
		u8*       d = static_cast<u8*>(dest);
		const u8* s = static_cast<const u8*>(src);
		while (count--) {
			*d++ = *s++;
		}
		return dest;
	}

	i32 MemCmp(const void* lhs, const void* rhs, size_t count) {
		const u8* l = static_cast<const u8*>(lhs);
		const u8* r = static_cast<const u8*>(rhs);
		while (count--) {
			if (*l != *r) {
				return static_cast<i32>(*l) - static_cast<i32>(*r);
			}
			++l;
			++r;
		}
		return 0;
	}

	void* MemFind(const void* ptr, size_t count, i32 value) {
		const u8* p = static_cast<const u8*>(ptr);
		u8        v = static_cast<u8>(value);
		while (count--) {
			if (*p == v) {
				return const_cast<u8*>(p);
			}
			++p;
		}
		return nullptr;
	}

	void* MemFindN(const void* ptr, size_t count, const void* value, size_t value_count) {
		const u8* p = static_cast<const u8*>(ptr);
		const u8* v = static_cast<const u8*>(value);
		while (count >= value_count) {
			if (MemCmp(p, v, value_count) == 0) {
				return const_cast<u8*>(p);
			}
			++p;
			--count;
		}
		return nullptr;
	}

	void* MemChr(const void* ptr, i32 value, size_t count) {
		const u8* p = static_cast<const u8*>(ptr);
		u8        v = static_cast<u8>(value);
		while (count--) {
			if (*p == v) {
				return const_cast<u8*>(p);
			}
			++p;
		}
		return nullptr;
	}

	size_t MemSpn(const void* dest, size_t dest_count, const void* src, size_t src_count) {
		const u8* d     = static_cast<const u8*>(dest);
		const u8* s     = static_cast<const u8*>(src);
		size_t    count = 0;
		while (count < dest_count) {
			bool found = false;
			for (size_t i = 0; i < src_count; ++i) {
				if (d[count] == s[i]) {
					found = true;
					break;
				}
			}
			if (!found)
				break;
			++count;
		}
		return count;
	}

	void* MemReverse(void* ptr, size_t count) {
		u8* start = static_cast<u8*>(ptr);
		u8* end   = start + count - 1;
		while (start < end) {
			u8 temp = *start;
			*start  = *end;
			*end    = temp;
			++start;
			--end;
		}
		return ptr;
	}
}