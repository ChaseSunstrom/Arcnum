#ifndef SPARK_STRING_HPP
#define SPARK_STRING_HPP

#include <core/util/classic/util.hpp>
#include <core/util/types.hpp>
#include <iostream>
#include <string>
#include "vector.hpp"

namespace Spark {
	class String {
	  public:
		class Iterator {
		  public:
			Iterator(char* ptr)
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
			char* m_ptr;
		};
		String()
			: m_data(nullptr) {}
		String(const char* data)
			: m_data(new char[strlen(data) + 1]) {
			strcpy(m_data, data);
		}
		String(const char* data, size_t length)
			: m_data(new char[length + 1]) {
			strncpy(m_data, data, length);
			m_data[length] = '\0';
		}
		String(const std::string& data)
			: m_data(new char[data.size() + 1]) {
			strcpy(m_data, data.c_str());
		}
		String(const String& other)
			: m_data(new char[strlen(other.m_data) + 1]) {
			strcpy(m_data, other.m_data);
		}

		~String() { delete[] m_data; }

		String& operator=(const String& other) {
			if (this != &other) {
				delete[] m_data;
				m_data = new char[strlen(other.m_data) + 1];
				strcpy(m_data, other.m_data);
			}
			return *this;
		}

		String& operator=(const std::string& other) {
			delete[] m_data;
			m_data = new char[other.size() + 1];
			strcpy(m_data, other.c_str());
			return *this;
		}

		String& operator=(const char* other) {
			delete[] m_data;
			m_data = new char[strlen(other) + 1];
			strcpy(m_data, other);
			return *this;
		}

		const char* CStr() const { return m_data; }

		std::string ToString() const { return std::string(m_data); }

		String& operator+=(const String& other) {
			char* tmp = new char[strlen(m_data) + strlen(other.m_data) + 1];
			strcpy(tmp, m_data);
			strcat(tmp, other.m_data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const std::string& other) {
			char* tmp = new char[strlen(m_data) + other.size() + 1];
			strcpy(tmp, m_data);
			strcat(tmp, other.c_str());
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const char* other) {
			char* tmp = new char[strlen(m_data) + strlen(other) + 1];
			strcpy(tmp, m_data);
			strcat(tmp, other);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const i8 data) {
			char* tmp = new char[strlen(m_data) + 2];
			strcpy(tmp, m_data);
			tmp[strlen(m_data)]     = data;
			tmp[strlen(m_data) + 1] = '\0';
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const i16 data) {
			char* tmp = new char[strlen(m_data) + 6];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%d", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const i32 data) {
			char* tmp = new char[strlen(m_data) + 12];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%d", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const i64 data) {
			char* tmp = new char[strlen(m_data) + 21];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%ld", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const u8 data) {
			char* tmp = new char[strlen(m_data) + 2];
			strcpy(tmp, m_data);
			tmp[strlen(m_data)]     = data;
			tmp[strlen(m_data) + 1] = '\0';
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const u16 data) {
			char* tmp = new char[strlen(m_data) + 6];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%u", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const u32 data) {
			char* tmp = new char[strlen(m_data) + 12];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%u", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const u64 data) {
			char* tmp = new char[strlen(m_data) + 21];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%lu", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const f32 data) {
			char* tmp = new char[strlen(m_data) + 12];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%f", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const f64 data) {
			char* tmp = new char[strlen(m_data) + 21];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%lf", data);
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String& operator+=(const bool data) {
			char* tmp = new char[strlen(m_data) + 6];
			strcpy(tmp, m_data);
			sprintf(tmp + strlen(m_data), "%s", data ? "true" : "false");
			delete[] m_data;
			m_data = tmp;
			return *this;
		}

		String operator+(const String& other) const {
			String result(*this);
			result += other;
			return result;
		}

		String operator+(const std::string& other) const {
			String result(*this);
			result += other;
			return result;
		}

		String operator+(const char* other) const {
			String result(*this);
			result += other;
			return result;
		}

		String operator+(const i8 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const i16 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const i32 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const i64 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const u8 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const u16 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const u32 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const u64 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const f32 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const f64 data) const {
			String result(*this);
			result += data;
			return result;
		}

		String operator+(const bool data) const {
			String result(*this);
			result += data;
			return result;
		}

		char operator[](size_t index) const { return m_data[index]; }

		bool operator==(const String& other) const { return StringCompare(m_data, other.m_data) == 0; }
		bool operator!=(const String& other) const { return StringCompare(m_data, other.m_data) != 0; }

		friend std::ostream& operator<<(std::ostream& os, const String& str) {
			os << str.m_data;
			return os;
		}

		size_t Length() const { return m_data ? StringLength(m_data) : 0; }

		// Check if the string is empty
		bool IsEmpty() const { return Length() == 0; }

		// Convert to lowercase
		String ToLower() const {
			String result(*this);
			for (char* p = result.m_data; *p; ++p) {
				*p = ToLowerChar(*p);
			}
			return result;
		}

		// Convert to uppercase
		String ToUpper() const {
			String result(*this);
			for (char* p = result.m_data; *p; ++p) {
				*p = ToUpperChar(*p);
			}
			return result;
		}

		// Trim whitespace from both ends
		String Trim() const {
			if (!m_data)
				return *this;
			const char* start = m_data;
			const char* end   = m_data + StringLength(m_data) - 1;
			while (start <= end && IsSpace(*start))
				++start;
			while (end >= start && IsSpace(*end))
				--end;
			return String(start, end - start + 1);
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
		bool StartsWith(const String& prefix) const {
			if (prefix.Length() > Length())
				return false;
			return StringCompare(m_data, prefix.m_data, prefix.Length()) == 0;
		}

		// Check if the string ends with a given suffix
		bool EndsWith(const String& suffix) const {
			if (suffix.Length() > Length())
				return false;
			return StringCompare(m_data + Length() - suffix.Length(), suffix.m_data) == 0;
		}

		// Find the position of a substring
		i32 Find(const String& substr, size_t pos = 0) const {
			if (pos >= Length())
				return -1;
			const char* found = StringFind(m_data + pos, substr.m_data);
			return found ? static_cast<i32>(found - m_data) : -1;
		}

		// Replace all occurrences of a substring
		String Replace(const String& from, const String& to) const {
			String  result(*this);
			size_t  start_pos = 0;
			int32_t find_pos;
			while ((find_pos = result.Find(from, start_pos)) != -1) {
				size_t end_pos = find_pos + from.Length();
				String prefix(result.m_data, find_pos);
				String suffix(result.m_data + end_pos);
				result    = prefix + to + suffix;
				start_pos = find_pos + to.Length();
			}
			return result;
		}

		// Split the string into a vector of substrings
		Vector<String> Split(const String& delimiter) const {
			Vector<String> result;
			size_t         start = 0;
			i32            end   = Find(delimiter);
			while (end != -1) {
				result.PushBack(String(m_data + start, end - start));
				start = end + delimiter.Length();
				end   = Find(delimiter, start);
			}
			result.PushBack(String(m_data + start));
			return result;
		}

		// Join a vector of strings with this string as a delimiter
		String Join(const Vector<String>& strings) const {
			if (strings.Empty())
				return String();
			String result = strings[0];
			for (size_t i = 1; i < strings.Size(); ++i) {
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
			for (const char* p = m_data; *p; ++p) {
				if (!IsDigit(*p))
					return false;
			}
			return true;
		}

		// Check if the string contains only alphabetic characters
		bool IsAlpha() const {
			for (const char* p = m_data; *p; ++p) {
				if (!IsAlpha(*p))
					return false;
			}
			return true;
		}

		// Check if the string contains only alphanumeric characters
		bool IsAlphanumeric() const {
			for (const char* p = m_data; *p; ++p) {
				if (!IsAlphanumeric(*p))
					return false;
			}
			return true;
		}

		// Reverse the string
		String Reverse() const {
			String result(*this);
			Spark::Reverse(result.m_data, result.m_data + result.Length());
			return result;
		}

		// Get a substring
		String Substring(size_t pos, size_t len = -1) const {
			if (pos >= Length())
				return String();
			if (len == -1 || pos + len > Length()) {
				len = Length() - pos;
			}
			return String(m_data + pos, len);
		}

	  private:
		static size_t      StringLength(const char* str);
		static char        ToLowerChar(char c);
		static char        ToUpperChar(char c);
		static bool        IsSpace(char c);
		static i32         StringCompare(const char* s1, const char* s2, size_t n = -1);
		static const char* StringFind(const char* str, const char* substr);
		static i64     StringToInt64(const char* str);
		static f32       StringToFloat(const char* str);
		static f64      StringToDouble(const char* str);
		static bool        IsDigit(char c);
		static bool        IsAlpha(char c);
		static bool        IsAlphanumeric(char c);

	  private:
		char* m_data;
	};
} // namespace Spark

#endif