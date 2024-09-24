#ifndef SPARK_STRING_HPP
#define SPARK_STRING_HPP

#include <core/util/classic/util.hpp>
#include <core/util/memory/allocator.hpp>
#include <core/util/types.hpp>
#include <iostream>
#include <string>
#include "vector.hpp"

namespace Spark {
	template<typename CharType = char, typename Allocator = _SPARK Allocator<CharType>> class BasicString {
	  public:
		using AllocatorType      = Allocator;
		using AllocatorTraits    = AllocatorTraits<AllocatorType>;

		using ValueType          = typename AllocatorTraits::ValueType;
		using PointerType        = typename AllocatorTraits::Pointer;
		using ReferenceType      = typename AllocatorTraits::Reference;
		using ConstReferenceType = typename AllocatorTraits::ConstReference;
		using SizeType           = typename AllocatorTraits::SizeType;
		using DifferenceType     = typename AllocatorTraits::DifferenceType;

		class Iterator {
		  public:
			using IteratorCategory   = std::random_access_iterator_tag;
			using ValueType          = typename AllocatorTraits::ValueType;
			using PointerType        = typename AllocatorTraits::Pointer;
			using ReferenceType      = typename AllocatorTraits::Reference;
			using ConstReferenceType = typename AllocatorTraits::ConstReference;
			using SizeType           = typename AllocatorTraits::SizeType;
			using DifferenceType     = typename AllocatorTraits::DifferenceType;

			Iterator(PointerType ptr)
				: m_ptr(ptr) {}

			// Prefix increment
			Iterator& operator++() {
				++m_ptr;
				return *this;
			}

			// Postfix increment
			Iterator operator++(i32) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			char& operator*() const { return *m_ptr; }

			bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }

			bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }

		  private:
			PointerType m_ptr;
		};
		BasicString()
			: m_data(nullptr) {}
		BasicString(const CharType* data)
			: m_data(AllocatorTraits::Allocate(strlen(data) + 1))
			, m_size(strlen(data)) {
			strcpy(m_data, data);
		}
		BasicString(const CharType* data, SizeType length)
			: m_data(AllocatorTraits::Allocate(length + 1))
			, m_size(length) {
			strncpy(m_data, data, length);
			m_data[length] = '\0';
		}
		BasicString(const std::string& data)
			: m_data(AllocatorTraits::Allocate(data.size() + 1))
			, m_size(data.size()) {
			strcpy(m_data, data.c_str());
		}
		BasicString(const BasicString& other)
			: m_data(AllocatorTraits::Allocate(other.m_size + 1))
			, m_size(other.m_size) {
			strcpy(m_data, other.m_data);
		}

		~BasicString() { AllocatorTraits::Deallocate(m_data); }

		BasicString& operator=(const BasicString& other) {
			if (this != &other) {
				AllocatorTraits::Deallocate(m_data);
				m_data = AllocatorTraits::Allocate(other.m_size + 1);
				m_size = other.m_size;
				strcpy(m_data, other.m_data);
			}
			return *this;
		}

		BasicString& operator=(const std::string& other) {
			AllocatorTraits::Deallocate(m_data);
			m_data = AllocatorTraits::Allocate(other.size() + 1);
			m_size = other.size();
			strcpy(m_data, other.c_str());
			return *this;
		}

		BasicString& operator=(const char* other) {
			AllocatorTraits::Deallocate(m_data);
			m_data = AllocatorTraits::Allocate(strlen(other) + 1);
			m_size = strlen(other);
			strcpy(m_data, other);
			return *this;
		}

		const char* CStr() const { return m_data; }

		std::string ToString() const { return std::string(m_data); }

		BasicString& operator+=(const BasicString& other) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + strlen(other.m_data) + 1);
			strcpy(tmp, m_data);
			strcat(tmp, other.m_data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const std::string& other) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + other.size() + 1);
			strcpy(tmp, m_data);
			strcat(tmp, other.c_str());
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const char* other) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + strlen(other) + 1);
			strcpy(tmp, m_data);
			strcat(tmp, other);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const i8 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 2);
			strcpy(tmp, m_data);
			tmp[strlen(m_data)]     = data;
			tmp[strlen(m_data) + 1] = '\0';
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const i16 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 6);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%d", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const i32 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 12);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%d", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const i64 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 21);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%ld", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const u8 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 2);
			strcpy(tmp, m_data);
			tmp[strlen(m_data)]     = data;
			tmp[strlen(m_data) + 1] = '\0';
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const u16 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 6);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%u", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const u32 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 12);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%u", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const u64 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 21);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%lu", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const f32 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 12);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%f", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const f64 data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 21);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%lf", data);
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString& operator+=(const bool data) {
			CharType* tmp = AllocatorTraits::Allocate(strlen(m_data) + 6);
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%s", data ? "true" : "false");
			AllocatorTraits::Deallocate(m_data);
			m_data = tmp;
			return *this;
		}

		BasicString operator+(const BasicString& other) const {
			BasicString result(*this);
			result += other;
			return result;
		}

		BasicString operator+(const std::string& other) const {
			BasicString result(*this);
			result += other;
			return result;
		}

		BasicString operator+(const char* other) const {
			BasicString result(*this);
			result += other;
			return result;
		}

		BasicString operator+(const i8 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const i16 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const i32 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const i64 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const u8 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const u16 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const u32 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const u64 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const f32 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const f64 data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		BasicString operator+(const bool data) const {
			BasicString result(*this);
			result += data;
			return result;
		}

		CharType operator[](SizeType index) const { return m_data[index]; }

		bool operator==(const BasicString& other) const { return StringCompare(m_data, other.m_data) == 0; }
		bool operator!=(const BasicString& other) const { return StringCompare(m_data, other.m_data) != 0; }

		friend std::ostream& operator<<(std::ostream& os, const BasicString& str) {
			os << str.m_data;
			return os;
		}

		SizeType Length() const { return m_data ? StringLength(m_data) : 0; }

		// Check if the string is empty
		bool IsEmpty() const { return Length() == 0; }

		// Convert to lowercase
		BasicString ToLower() const {
			BasicString result(*this);
			for (CharType* p = result.m_data; *p; ++p) {
				*p = ToLowerChar(*p);
			}
			return result;
		}

		// Convert to uppercase
		BasicString ToUpper() const {
			BasicString result(*this);
			for (CharType* p = result.m_data; *p; ++p) {
				*p = ToUpperChar(*p);
			}
			return result;
		}

		// Trim whitespace from both ends
		BasicString Trim() const {
			if (!m_data)
				return *this;
			const CharType* start = m_data;
			const CharType* end   = m_data + StringLength(m_data) - 1;
			while (start <= end && IsSpace(*start))
				++start;
			while (end >= start && IsSpace(*end))
				--end;
			return BasicString(start, end - start + 1);
		}

		Iterator begin() { return Iterator(m_data); }

		Iterator end() { return Iterator(m_data + Length()); }

		const Iterator begin() const { return Iterator(m_data); }

		const Iterator end() const { return Iterator(m_data + Length()); }

		Iterator Begin() { return Iterator(m_data); }

		Iterator End() { return Iterator(m_data + Length()); }

		const Iterator Begin() const { return Iterator(m_data); }

		const Iterator End() const { return Iterator(m_data + Length()); }

		// Check if the string starts with a given prefix
		bool StartsWith(const BasicString& prefix) const {
			if (prefix.Length() > Length())
				return false;
			return StringCompare(m_data, prefix.m_data, prefix.Length()) == 0;
		}

		// Check if the string ends with a given suffix
		bool EndsWith(const BasicString& suffix) const {
			if (suffix.Length() > Length())
				return false;
			return StringCompare(m_data + Length() - suffix.Length(), suffix.m_data) == 0;
		}

		// Find the position of a substring
		i32 Find(const BasicString& substr, SizeType pos = 0) const {
			if (pos >= Length())
				return -1;
			const CharType* found = StringFind(m_data + pos, substr.m_data);
			return found ? static_cast<i32>(found - m_data) : -1;
		}

		// Replace all occurrences of a substring
		BasicString Replace(const BasicString& from, const BasicString& to) const {
			BasicString  result(*this);
			SizeType  start_pos = 0;
			int32_t find_pos;
			while ((find_pos = result.Find(from, start_pos)) != -1) {
				SizeType end_pos = find_pos + from.Length();
				BasicString prefix(result.m_data, find_pos);
				BasicString suffix(result.m_data + end_pos);
				result    = prefix + to + suffix;
				start_pos = find_pos + to.Length();
			}
			return result;
		}

		// Split the string into a vector of substrings
		Vector<BasicString> Split(const BasicString& delimiter) const {
			Vector<BasicString> result;
			SizeType         start = 0;
			i32            end   = Find(delimiter);
			while (end != -1) {
				result.PushBack(BasicString(m_data + start, end - start));
				start = end + delimiter.Length();
				end   = Find(delimiter, start);
			}
			result.PushBack(BasicString(m_data + start));
			return result;
		}

		// Join a vector of strings with this string as a delimiter
		BasicString Join(const Vector<BasicString>& strings) const {
			if (strings.Empty())
				return BasicString();
			BasicString result = strings[0];
			for (SizeType i = 1; i < strings.Size(); ++i) {
				result += *this + strings[i];
			}
			return result;
		}

		// Convert to integer
		i64 ToInt() const { return StringToInt64(m_data); }

		// Convert to float
		f32 ToFloat() const { return StringToFloat(m_data); }

		// Convert to double
		f64 ToDouble() const { return StringToDouble(m_data); }

		// Check if the string contains only digits
		bool IsNumeric() const {
			for (const CharType* p = m_data; *p; ++p) {
				if (!IsDigit(*p))
					return false;
			}
			return true;
		}

		// Check if the string contains only alphabetic characters
		bool IsAlpha() const {
			for (const CharType* p = m_data; *p; ++p) {
				if (!IsAlpha(*p))
					return false;
			}
			return true;
		}

		// Check if the string contains only alphanumeric characters
		bool IsAlphanumeric() const {
			for (const CharType* p = m_data; *p; ++p) {
				if (!IsAlphanumeric(*p))
					return false;
			}
			return true;
		}

		// Reverse the string
		BasicString Reverse() const {
			BasicString result(*this);
			_SPARK Reverse(result.m_data, result.m_data + result.Length());
			return result;
		}

		// Get a substring
		BasicString Substring(SizeType pos, SizeType len = -1) const {
			if (pos >= Length())
				return BasicString();
			if (len == -1 || pos + len > Length()) {
				len = Length() - pos;
			}
			return BasicString(m_data + pos, len);
		}

	  private:
		static SizeType StringLength(const char* str) {
			SizeType len = 0;
			while (*str++)
				++len;
			return len;
		}

		static CharType ToLowerChar(CharType c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }

		static CharType ToUpperChar(CharType c) { return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c; }

		static bool IsSpace(CharType c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }

		static i32 StringCompare(const CharType* s1, const CharType* s2, SizeType n) {
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

		const CharType* StringFind(const CharType* str, const CharType* substr) {
			while (*str) {
				const CharType* s1 = str;
				const CharType* s2 = substr;
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

		i64 StringToInt64(const CharType* str) {
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

		f32 StringToFloat(const CharType* str) {
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

		f64 StringToDouble(const CharType* str) {
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

		bool IsDigit(CharType c) { return c >= '0' && c <= '9'; }

		bool IsAlpha(CharType c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

		bool IsAlphanumeric(CharType c) { return IsAlpha(c) || IsDigit(c); }

	  private:
		CharType* m_data;
		SizeType  m_size;
		Allocator m_allocator;
	};

	using String = BasicString<char, Allocator<char>>;

} // namespace Spark

namespace std {
	template<> struct hash<_SPARK String> {
		size_t operator()(const _SPARK String& str) const noexcept { return hash<const char*>()(str.CStr()); }
	};
} // namespace std

#endif