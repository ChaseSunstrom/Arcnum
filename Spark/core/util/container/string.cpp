#include "string.hpp"

namespace Spark
{

	size_t  String::StringLength(const char* str) {
		size_t  len = 0;
		while (*str++)
			++len;
		return len;
	}

	char String::ToLowerChar(char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }

	char String::ToUpperChar(char c) { return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c; }

	bool String::IsSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }

	i32 String::StringCompare(const char* s1, const char* s2, size_t  n) {
		if (n == -1) {
			while (*s1 && (*s1 == *s2)) {
				++s1;
				++s2;
			}
			return *(const unsigned char*) s1 - *(const unsigned char*) s2;
		} else {
			while (n-- && *s1 && (*s1 == *s2)) {
				++s1;
				++s2;
			}
			return n == -1 ? 0 : *(const unsigned char*) s1 - *(const unsigned char*) s2;
		}
	}

	const char* String::StringFind(const char* str, const char* substr) {
		while (*str) {
			const char* s1 = str;
			const char* s2 = substr;
			while (*s1 && *s2 && (*s1 == *s2)) {
				++s1;
				++s2;
			}
			if (!*s2)
				return str;
			++str;
		}
		return nullptr;
	}

	i64 String::StringToInt64(const char* str) {
		i64 result = 0;
		i32 sign   = 1;
		if (*str == '-') {
			sign = -1;
			++str;
		}
		while (IsDigit(*str)) {
			result = result * 10 + (*str - '0');
			++str;
		}
		return sign * result;
	}

	f32 String::StringToFloat(const char* str) {
		f32  result   = 0;
		f32  fraction = 0;
		i32  sign     = 1;
		bool decimal  = false;
		f32  div      = 1.0f;

		if (*str == '-') {
			sign = -1;
			++str;
		}

		while (*str) {
			if (*str == '.') {
				decimal = true;
			} else if (IsDigit(*str)) {
				if (decimal) {
					div *= 10.0f;
					fraction = fraction + (*str - '0') / div;
				} else {
					result = result * 10.0f + (*str - '0');
				}
			} else {
				break;
			}
			++str;
		}

		return sign * (result + fraction);
	}

	f64 String::StringToDouble(const char* str) {
		f64  result   = 0;
		f64  fraction = 0;
		i32  sign     = 1;
		bool decimal  = false;
		f64  div      = 1.0;

		if (*str == '-') {
			sign = -1;
			++str;
		}

		while (*str) {
			if (*str == '.') {
				decimal = true;
			} else if (IsDigit(*str)) {
				if (decimal) {
					div *= 10.0;
					fraction = fraction + (*str - '0') / div;
				} else {
					result = result * 10.0 + (*str - '0');
				}
			} else {
				break;
			}
			++str;
		}

		return sign * (result + fraction);
	}

	bool String::IsDigit(char c) { return c >= '0' && c <= '9'; }

	bool String::IsAlpha(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

	bool String::IsAlphanumeric(char c) { return IsAlpha(c) || IsDigit(c); }
}