#ifndef SPARK_STRING_HPP
#define SPARK_STRING_HPP

#include "vector.hpp"
#include <core/util/classic/hash.hpp>
#include <core/util/classic/util.hpp>
#include <core/util/memory/allocator.hpp>
#include <core/util/memory/memory_util.hpp>
#include <core/util/types.hpp>
#include <iostream>
#include <string>

namespace Spark {

	// Forward declaration of BasicString
	template<typename CharType, typename Allocator> class BasicString;

	// Iterator class definition
	template<typename CharType, bool IsConst> class BasicStringIterator {
	  public:
		using IteratorCategory = RandomAccessIteratorTag;
		using ValueType        = CharType;
		using DifferenceType   = ptrdiff_t;
		using Pointer          = ConditionalT<IsConst, const CharType*, CharType*>;
		using Reference        = ConditionalT<IsConst, const CharType&, CharType&>;

		BasicStringIterator(Pointer ptr)
			: m_ptr(ptr) {}

		// Prefix increment
		BasicStringIterator& operator++() {
			++m_ptr;
			return *this;
		}

		// Postfix increment
		BasicStringIterator operator++(int) {
			BasicStringIterator tmp = *this;
			++(*this);
			return tmp;
		}

		// Prefix decrement
		BasicStringIterator& operator--() {
			--m_ptr;
			return *this;
		}

		// Postfix decrement
		BasicStringIterator operator--(int) {
			BasicStringIterator tmp = *this;
			--(*this);
			return tmp;
		}

		// Addition operator
		BasicStringIterator operator+(DifferenceType n) const { return BasicStringIterator(m_ptr + n); }

		// Addition assignment
		BasicStringIterator& operator+=(DifferenceType n) {
			m_ptr += n;
			return *this;
		}

		// Subtraction operator
		BasicStringIterator operator-(DifferenceType n) const { return BasicStringIterator(m_ptr - n); }

		// Subtraction assignment
		BasicStringIterator& operator-=(DifferenceType n) {
			m_ptr -= n;
			return *this;
		}

		// Difference between iterators
		DifferenceType operator-(const BasicStringIterator& other) const { return m_ptr - other.m_ptr; }

		// Dereference operator
		Reference operator*() const { return *m_ptr; }

		// Arrow operator
		Pointer operator->() const { return m_ptr; }

		// Subscript operator
		Reference operator[](DifferenceType n) const { return *(m_ptr + n); }

		// Equality comparison
		bool operator==(const BasicStringIterator& other) const { return m_ptr == other.m_ptr; }

		// Inequality comparison
		bool operator!=(const BasicStringIterator& other) const { return m_ptr != other.m_ptr; }

		// Less than comparison
		bool operator<(const BasicStringIterator& other) const { return m_ptr < other.m_ptr; }

		// Greater than comparison
		bool operator>(const BasicStringIterator& other) const { return m_ptr > other.m_ptr; }

		// Less than or equal comparison
		bool operator<=(const BasicStringIterator& other) const { return m_ptr <= other.m_ptr; }

		// Greater than or equal comparison
		bool operator>=(const BasicStringIterator& other) const { return m_ptr >= other.m_ptr; }

	  private:
		Pointer m_ptr;
	};

	template<typename CharType = char, typename Allocator = _SPARK Allocator<CharType>> class BasicString {
	  public:
		using AllocatorType   = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using ValueType       = typename AllocatorTraits::ValueType;
		using Pointer         = typename AllocatorTraits::Pointer;
		using Reference       = typename AllocatorTraits::Reference;
		using ConstReference  = typename AllocatorTraits::ConstReference;
		using SizeType        = typename AllocatorTraits::SizeType;
		using DifferenceType  = typename AllocatorTraits::DifferenceType;

		using Iterator        = BasicStringIterator<CharType, false>;
		using ConstIterator   = BasicStringIterator<CharType, true>;

		BasicString()
			: m_data(nullptr)
			, m_size(0)
			, m_capacity(1) {
			m_data    = AllocatorTraits::Allocate(m_allocator, m_capacity);
			m_data[0] = '\0';
		}

		BasicString(const CharType* data)
			: m_size(StringLength(data))
			, m_capacity(m_size + 1) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			StringCopy(m_data, data, m_capacity);
			m_data[m_size] = '\0';
		}

		BasicString(const CharType* data, SizeType length)
			: m_size(length)
			, m_capacity(length + 1) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			StringCopyN(m_data, data, length);
			m_data[length] = '\0';
		}

		BasicString(const std::string& data)
			: m_size(data.size())
			, m_capacity(m_size + 1) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			StringCopy(m_data, data.c_str(), m_capacity);
			m_data[m_size] = '\0';
		}

		BasicString(const BasicString& other)
			: m_size(other.m_size)
			, m_capacity(other.m_capacity)
			, m_allocator(other.m_allocator) {
			m_data = AllocatorTraits::Allocate(m_allocator, m_capacity);
			StringCopy(m_data, other.m_data, m_capacity);
			m_data[m_size] = '\0';
		}

		BasicString(BasicString&& other) noexcept
			: m_data(other.m_data)
			, m_size(other.m_size)
			, m_capacity(other.m_capacity)
			, m_allocator(Move(other.m_allocator)) {
			other.m_data     = nullptr;
			other.m_size     = 0;
			other.m_capacity = 0;
		}

		~BasicString() {
			if (m_data) {
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			}
		}

		BasicString& operator=(const BasicString& other) {
			if (this != &other) {
				BasicString tmp(other);
				Swap(tmp);
			}
			return *this;
		}

		BasicString& operator=(BasicString&& other) noexcept {
			if (this != &other) {
				// First deallocate our existing memory
				if (m_data) {
					AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
				}
        
				// Then take ownership of other's resources
				m_data = other.m_data;
				m_size = other.m_size;
				m_capacity = other.m_capacity;
				m_allocator = Move(other.m_allocator);
        
				// Reset other to empty state
				other.m_data = nullptr;
				other.m_size = 0;
				other.m_capacity = 0;
			}
			return *this;
		}

		BasicString& operator=(const std::string& other) {
			BasicString tmp(other);
			Swap(tmp);
			return *this;
		}

		BasicString& operator=(const CharType* other) {
			BasicString tmp(other);
			Swap(tmp);
			return *this;
		}

		void Swap(BasicString& other) noexcept {
			_SPARK Swap(m_data, other.m_data);
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_capacity, other.m_capacity);
			_SPARK Swap(m_allocator, other.m_allocator);
		}

		const CharType* CStr() const { return m_data ? m_data : ""; }

		std::string ToString() const { return m_data ? std::string(m_data) : std::string(); }

		BasicString& operator+=(const BasicString& other) {
			Append(other.m_data, other.m_size);
			return *this;
		}

		BasicString& operator+=(const std::string& other) {
			Append(other.c_str(), other.size());
			return *this;
		}

		BasicString& operator+=(const CharType* other) {
			Append(other, StringLength(other));
			return *this;
		}

		BasicString& operator+=(const bool value) { return *this += (value ? "true" : "false"); }

		BasicString& operator+=(const CharType ch) {
			Append(&ch, 1);
			return *this;
		}

		template<typename _Ty> BasicString& operator+=(_Ty value) { return *this += ToString(value); }

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

		BasicString operator+(const CharType* other) const {
			BasicString result(*this);
			result += other;
			return result;
		}

		BasicString operator+(const bool value) const {
			BasicString result(*this);
			result += (value ? "true" : "false");
			return result;
		}

		BasicString operator+(const CharType ch) const {
			BasicString result(*this);
			result += ch;
			return result;
		}

		template<typename _Ty> BasicString operator+(_Ty value) const {
			BasicString result(*this);
			result += value;
			return result;
		}

		CharType operator[](SizeType index) const { return (index < m_size) ? m_data[index] : '\0'; }

		Reference operator[](SizeType index) { return m_data[index]; }

		bool operator==(const BasicString& other) const { return (m_size == other.m_size) && (StringCompare(m_data, other.m_data) == 0); }

		bool operator!=(const BasicString& other) const { return !(*this == other); }

		bool operator<(const BasicString& other) const { return StringCompare(m_data, other.m_data) < 0; }

		bool operator>(const BasicString& other) const { return StringCompare(m_data, other.m_data) > 0; }

		bool operator<=(const BasicString& other) const { return StringCompare(m_data, other.m_data) <= 0; }

		bool operator>=(const BasicString& other) const { return StringCompare(m_data, other.m_data) >= 0; }

		friend std::ostream& operator<<(std::ostream& os, const BasicString& str) {
			os << (str.m_data ? str.m_data : "");
			return os;
		}

		SizeType Length() const { return m_size; }
		SizeType Size() const { return m_size; }
		SizeType Capacity() const { return m_capacity; }

		bool IsEmpty() const { return m_size == 0; }

		BasicString ToLower() const {
			BasicString result(*this);
			for (CharType* p = result.m_data; *p; ++p) {
				*p = ToLowerChar(*p);
			}
			return result;
		}

		BasicString ToUpper() const {
			BasicString result(*this);
			for (CharType* p = result.m_data; *p; ++p) {
				*p = ToUpperChar(*p);
			}
			return result;
		}

		BasicString Trim() const {
			if (!m_data || m_size == 0)
				return *this;
			const CharType* start = m_data;
			const CharType* end   = m_data + m_size - 1;
			while (start <= end && IsSpace(*start))
				++start;
			while (end >= start && IsSpace(*end))
				--end;
			return BasicString(start, end - start + 1);
		}

		Iterator      begin() { return Iterator(m_data); }
		Iterator      end() { return Iterator(m_data + m_size); }
		ConstIterator begin() const { return ConstIterator(m_data); }
		ConstIterator end() const { return ConstIterator(m_data + m_size); }
		Iterator      Begin() { return begin(); }
		Iterator      End() { return end(); }
		ConstIterator Begin() const { return begin(); }
		ConstIterator End() const { return end(); }

		bool StartsWith(const BasicString& prefix) const {
			if (prefix.m_size > m_size)
				return false;
			return StringCompare(m_data, prefix.m_data, prefix.m_size) == 0;
		}

		bool EndsWith(const BasicString& suffix) const {
			if (suffix.m_size > m_size)
				return false;
			return StringCompare(m_data + m_size - suffix.m_size, suffix.m_data) == 0;
		}

		SizeType Find(const BasicString& substr, SizeType pos = 0) const {
			if (pos >= m_size)
				return npos;
			const CharType* found = StringFind(m_data + pos, substr.m_data);
			return found ? static_cast<SizeType>(found - m_data) : npos;
		}

		BasicString Replace(const BasicString& from, const BasicString& to) const {
			BasicString result(*this);
			SizeType    start_pos = 0;
			SizeType    find_pos;
			while ((find_pos = result.Find(from, start_pos)) != npos) {
				result.ReplaceInternal(find_pos, from.m_size, to);
				start_pos = find_pos + to.m_size;
			}
			return result;
		}

		Vector<BasicString> Split(const BasicString& delimiter) const {
			Vector<BasicString> result;
			SizeType            start = 0;
			SizeType            end   = Find(delimiter);
			while (end != npos) {
				result.PushBack(Substring(start, end - start));
				start = end + delimiter.m_size;
				end   = Find(delimiter, start);
			}
			result.PushBack(Substring(start));
			return result;
		}

		BasicString Join(const Vector<BasicString>& strings) const {
			if (strings.Empty())
				return BasicString();
			BasicString result = strings[0];
			for (SizeType i = 1; i < strings.Size(); ++i) {
				result += *this + strings[i];
			}
			return result;
		}

		i64 ToInt() const { return StringToInt64(m_data); }
		f32 ToFloat() const { return StringToFloat(m_data); }
		f64 ToDouble() const { return StringToDouble(m_data); }

		bool IsNumeric() const {
			if (IsEmpty())
				return false;
			for (const CharType* p = m_data; *p; ++p) {
				if (!IsDigit(*p))
					return false;
			}
			return true;
		}

		bool IsAlpha() const {
			if (IsEmpty())
				return false;
			for (const CharType* p = m_data; *p; ++p) {
				if (!IsAlpha(*p))
					return false;
			}
			return true;
		}

		bool IsAlphanumeric() const {
			if (IsEmpty())
				return false;
			for (const CharType* p = m_data; *p; ++p) {
				if (!IsAlphanumeric(*p))
					return false;
			}
			return true;
		}

		BasicString Reverse() const {
			BasicString result(*this);
			_SPARK      Reverse(result.m_data, result.m_data + result.m_size);
			return result;
		}

		BasicString Substring(SizeType pos, SizeType len = npos) const {
			if (pos >= m_size)
				return BasicString();
			if (len == npos || pos + len > m_size) {
				len = m_size - pos;
			}
			return BasicString(m_data + pos, len);
		}

		void Reserve(SizeType new_capacity) {
			if (new_capacity > m_capacity) {
				ReallocateAndCopy(new_capacity);
			}
		}

		void Resize(SizeType new_size, CharType ch = CharType()) {
			if (new_size > m_size) {
				if (new_size > m_capacity) {
					ReallocateAndCopy(new_size);
				}
				_SPARK Fill(m_data + m_size, m_data + new_size, ch);
			}
			m_size         = new_size;
			m_data[m_size] = '\0';
		}

		void Clear() {
			m_size = 0;
			if (m_data) {
				m_data[0] = '\0';
			}
		}

		void ShrinkToFit() {
			if (m_capacity > m_size + 1) {
				ReallocateAndCopy(m_size + 1);
			}
		}

	  private:
		static constexpr SizeType npos = static_cast<SizeType>(-1);

		static SizeType StringLength(const CharType* str) {
			SizeType len = 0;
			while (*str++)
				++len;
			return len;
		}

		static CharType ToLowerChar(CharType c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }
		static CharType ToUpperChar(CharType c) { return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c; }
		static bool     IsSpace(CharType c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }

		static i32 StringCompare(const CharType* s1, const CharType* s2, SizeType n = npos) {
			if (n == npos) {
				while (*s1 && (*s1 == *s2)) {
					++s1;
					++s2;
				}
				return (u8) *s1 - (u8) *s2;
			} else {
				for (SizeType i = 0; i < n; ++i) {
					if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
						return (u8) s1[i] - (u8) s2[i];
					}
				}
				return 0;
			}
		}

		static const CharType* StringFind(const CharType* str, const CharType* substr) {
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

		static void StringCopy(CharType* dest, const CharType* src, SizeType n) {
			while (n-- && (*dest++ = *src++))
				;
		}

		static void StringCopyN(CharType* dest, const CharType* src, SizeType n) { MemCpy(dest, src, n * sizeof(CharType)); }

		static i64 StringToInt64(const CharType* str) {
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

		static f32 StringToFloat(const CharType* str) {
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

		static f64 StringToDouble(const CharType* str) {
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

		static bool IsDigit(CharType c) { return c >= '0' && c <= '9'; }
		static bool IsAlpha(CharType c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
		static bool IsAlphanumeric(CharType c) { return IsAlpha(c) || IsDigit(c); }

		void ReallocateAndCopy(SizeType new_capacity) {
			CharType* new_data = AllocatorTraits::Allocate(m_allocator, new_capacity);
			if (m_data) {
				StringCopyN(new_data, m_data, m_size);
				new_data[m_size] = '\0'; // Ensure null-termination
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
			} else {
				new_data[0] = '\0';
			}
			m_data     = new_data;
			m_capacity = new_capacity;
		}

		void Append(const CharType* str, SizeType len) {
			SizeType new_size = m_size + len;
			if (new_size >= m_capacity) {
				SizeType new_capacity = (new_size + 1) * 2;
				ReallocateAndCopy(new_capacity);
			}
			MemCpy(m_data + m_size, str, len * sizeof(CharType));
			m_size         = new_size;
			m_data[m_size] = '\0'; // Ensure null-termination
		}

		void ReplaceInternal(SizeType pos, SizeType len, const BasicString& str) {
			SizeType new_size = m_size - len + str.m_size;
			if (new_size >= m_capacity) {
				SizeType  new_capacity = (new_size + 1) * 2;
				CharType* new_data     = AllocatorTraits::Allocate(m_allocator, new_capacity);
				StringCopyN(new_data, m_data, pos);
				StringCopyN(new_data + pos, str.m_data, str.m_size);
				StringCopyN(new_data + pos + str.m_size, m_data + pos + len, m_size - pos - len);
				AllocatorTraits::Deallocate(m_allocator, m_data, m_capacity);
				m_data     = new_data;
				m_capacity = new_capacity;
			} else {
				if (len != str.m_size) {
					MemMove(m_data + pos + str.m_size, m_data + pos + len, (m_size - pos - len) * sizeof(CharType));
				}
				StringCopyN(m_data + pos, str.m_data, str.m_size);
			}
			m_size         = new_size;
			m_data[m_size] = '\0';
		}

		template<typename _Ty> static BasicString ToString(_Ty value) {
			std::ostringstream oss;
			oss << value;
			return BasicString(oss.str().c_str());
		}

	  private:
		CharType* m_data;
		SizeType  m_size;
		SizeType  m_capacity;
		Allocator m_allocator;
	};

	// Global operator+ overloads for String concatenation
	template<typename CharType, typename Allocator> BasicString<CharType, Allocator> operator+(const CharType* lhs, const BasicString<CharType, Allocator>& rhs) {
		BasicString<CharType, Allocator> result(lhs);
		result += rhs;
		return result;
	}

	template<typename CharType, typename Allocator> BasicString<CharType, Allocator> operator+(const std::string& lhs, const BasicString<CharType, Allocator>& rhs) {
		BasicString<CharType, Allocator> result(lhs);
		result += rhs;
		return result;
	}

	template<typename CharType, typename Allocator> BasicString<CharType, Allocator> operator+(CharType lhs, const BasicString<CharType, Allocator>& rhs) {
		BasicString<CharType, Allocator> result(&lhs, 1);
		result += rhs;
		return result;
	}

	using String = BasicString<char, Allocator<char>>;

	inline void Swap(String& lhs, String& rhs) { lhs.Swap(rhs); }

	template<> struct Hash<String> {
		usize operator()(const String& str) const noexcept { return Hash<const char*>()(str.CStr()); }
	};
} // namespace Spark

namespace std {
	// STD overload for String
	template<> struct hash<_SPARK String> {
		size_t operator()(const _SPARK String& str) const noexcept { return hash<const char*>()(str.CStr()); }
	};
} // namespace std

#endif
