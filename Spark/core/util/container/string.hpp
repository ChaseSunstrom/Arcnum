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
		using Pointer            = typename AllocatorTraits::Pointer;
		using Reference          = typename AllocatorTraits::Reference;
		using ConstReference     = typename AllocatorTraits::ConstReference;
		using SizeType           = typename AllocatorTraits::SizeType;
		using DifferenceType     = typename AllocatorTraits::DifferenceType;

		class Iterator {
		  public:
			using IteratorCategory   = std::random_access_iterator_tag;
			using ValueType          = typename AllocatorTraits::ValueType;
			using Pointer            = typename AllocatorTraits::Pointer;
			using Reference          = typename AllocatorTraits::Reference;
			using ConstReference     = typename AllocatorTraits::ConstReference;
			using SizeType           = typename AllocatorTraits::SizeType;
			using DifferenceType     = typename AllocatorTraits::DifferenceType;

			Iterator(Pointer ptr)
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
			Pointer m_ptr;
		};
		BasicString()
			: m_data(nullptr), m_size(0), m_capacity(0) {}
		BasicString(const CharType* data)
			: m_size(strlen(data)), m_capacity(m_size + 1) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			strcpy(m_data, data);
		}
		BasicString(const CharType* data, SizeType length)
			: m_size(length), m_capacity(length + 1) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			strncpy(m_data, data, length);
			m_data[length] = '\0';
		}
		BasicString(const std::string& data)
			: m_size(data.size()), m_capacity(m_size + 1) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			strcpy(m_data, data.c_str());
		}
		BasicString(const BasicString& other)
			: m_size(other.m_size), m_capacity(other.m_capacity) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			strcpy(m_data, other.m_data);
		}

		~BasicString() {
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
		}

		BasicString& operator=(const BasicString& other) {
			if (this != &other) {
				if (m_data) {
					AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
				}
				m_size = other.m_size;
				m_capacity = other.m_capacity;
				m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
				strcpy(m_data, other.m_data);
			}
			return *this;
		}

		BasicString& operator=(const std::string& other) {
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_size = other.size();
			m_capacity = m_size + 1;
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			strcpy(m_data, other.c_str());
			return *this;
		}

		BasicString& operator=(const char* other) {
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_size = strlen(other);
			m_capacity = m_size + 1;
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			strcpy(m_data, other);
			return *this;
		}

		const char* CStr() const { return m_data; }

		std::string ToString() const { return std::string(m_data); }

		BasicString& operator+=(const BasicString& other) {
			SizeType new_size = m_size + other.m_size;
			SizeType new_capacity = new_size + 1;
			CharType* tmp = AllocatorTraits::Allocate(m_allocator, new_capacity);
			strcpy(tmp, m_data);
			strcat(tmp, other.m_data);
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_data = tmp;
			m_size = new_size;
			m_capacity = new_capacity;
			return *this;
		}

		BasicString& operator+=(const std::string& other) {
			SizeType new_size = m_size + other.size();
			SizeType new_capacity = new_size + 1;
			CharType* tmp = AllocatorTraits::Allocate(m_allocator, new_capacity);
			strcpy(tmp, m_data);
			strcat(tmp, other.c_str());
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_data = tmp;
			m_size = new_size;
			m_capacity = new_capacity;
			return *this;
		}

		BasicString& operator+=(const char* other) {
			SizeType other_size = strlen(other);
			SizeType new_size = m_size + other_size;
			SizeType new_capacity = new_size + 1;
			CharType* tmp = AllocatorTraits::Allocate(m_allocator, new_capacity);
			strcpy(tmp, m_data);
			strcat(tmp, other);
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_data = tmp;
			m_size = new_size;
			m_capacity = new_capacity;
			return *this;
		}

		BasicString& operator+=(const i8 data) {
			SizeType new_size = m_size + 1;
			SizeType new_capacity = new_size + 1;
			CharType* tmp = AllocatorTraits::Allocate(m_allocator, new_capacity);
			strcpy(tmp, m_data);
			tmp[m_size] = data;
			tmp[new_size] = '\0';
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_data = tmp;
			m_size = new_size;
			m_capacity = new_capacity;
			return *this;
		}

		BasicString& operator+=(const i16 data) {
			char buffer[7];
			sprintf(buffer, "%d", data);
			return *this += buffer;
		}

		BasicString& operator+=(const i32 data) {
			char buffer[12];
			sprintf(buffer, "%d", data);
			return *this += buffer;
		}

		BasicString& operator+=(const i64 data) {
			char buffer[21];
			sprintf(buffer, "%ld", data);
			return *this += buffer;
		}

		BasicString& operator+=(const u8 data) {
			SizeType new_size = m_size + 1;
			SizeType new_capacity = new_size + 1;
			CharType* tmp = AllocatorTraits::Allocate(m_allocator, new_capacity);
			strcpy(tmp, m_data);
			tmp[m_size] = data;
			tmp[new_size] = '\0';
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
			m_data = tmp;
			m_size = new_size;
			m_capacity = new_capacity;
			return *this;
		}

		BasicString& operator+=(const u16 data) {
			char buffer[6];
			sprintf(buffer, "%u", data);
			return *this += buffer;
		}

		BasicString& operator+=(const u32 data) {
			char buffer[11];
			sprintf(buffer, "%u", data);
			return *this += buffer;
		}

		BasicString& operator+=(const u64 data) {
			char buffer[21];
			sprintf(buffer, "%lu", data);
			return *this += buffer;
		}

		BasicString& operator+=(const f32 data) {
			char buffer[32];
			sprintf(buffer, "%f", data);
			return *this += buffer;
		}

		BasicString& operator+=(const f64 data) {
			char buffer[32];
			sprintf(buffer, "%lf", data);
			return *this += buffer;
		}

		BasicString& operator+=(const bool data) {
			return *this += (data ? "true" : "false");
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

		SizeType Length() const { return m_size; }

		// Check if the string is empty
		bool IsEmpty() const { return m_size == 0; }

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
			const CharType* end   = m_data + m_size - 1;
			while (start <= end && IsSpace(*start))
				++start;
			while (end >= start && IsSpace(*end))
				--end;
			return BasicString(start, end - start + 1);
		}

		Iterator begin() { return Iterator(m_data); }

		Iterator end() { return Iterator(m_data + m_size); }

		const Iterator begin() const { return Iterator(m_data); }

		const Iterator end() const { return Iterator(m_data + m_size); }

		Iterator Begin() { return Iterator(m_data); }

		Iterator End() { return Iterator(m_data + m_size); }

		const Iterator Begin() const { return Iterator(m_data); }

		const Iterator End() const { return Iterator(m_data + m_size); }

		// Check if the string starts with a given prefix
		bool StartsWith(const BasicString& prefix) const {
			if (prefix.m_size > m_size)
				return false;
			return StringCompare(m_data, prefix.m_data, prefix.m_size) == 0;
		}

		// Check if the string ends with a given suffix
		bool EndsWith(const BasicString& suffix) const {
			if (suffix.m_size > m_size)
				return false;
			return StringCompare(m_data + m_size - suffix.m_size, suffix.m_data) == 0;
		}

		// Find the position of a substring
		i32 Find(const BasicString& substr, SizeType pos = 0) const {
			if (pos >= m_size)
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
				SizeType end_pos = find_pos + from.m_size;
				BasicString prefix(result.m_data, find_pos);
				BasicString suffix(result.m_data + end_pos);
				result    = prefix + to + suffix;
				start_pos = find_pos + to.m_size;
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
				start = end + delimiter.m_size;
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
			_SPARK Reverse(result.m_data, result.m_data + result.m_size);
			return result;
		}

		// Get a substring
		BasicString Substring(SizeType pos, SizeType len = -1) const {
			if (pos >= m_size)
				return BasicString();
			if (len == -1 || pos + len > m_size) {
				len = m_size - pos;
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

		static i32 StringCompare(const CharType* s1, const CharType* s2, SizeType n = -1) {
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
		SizeType  m_capacity;
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