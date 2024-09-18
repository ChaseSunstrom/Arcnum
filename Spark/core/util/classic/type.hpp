#ifndef SPARK_TYPE_HPP
#define SPARK_TYPE_HPP

#include <core/util/types.hpp>
#include <core/util/defines.hpp>
#include <typeinfo>

namespace Spark {
	struct TypeInfoData {
		const char* undecorated_name;
		const char* decorated_name;

		TypeInfoData(const char* undecorated, const char* decorated)
			: undecorated_name(undecorated)
			, decorated_name(decorated) {}

		TypeInfoData()                               = delete;
		TypeInfoData(const TypeInfoData&)            = delete;
		TypeInfoData(TypeInfoData&&)                 = delete;
		TypeInfoData& operator=(const TypeInfoData&) = delete;
		TypeInfoData& operator=(TypeInfoData&&)      = delete;
	};

	i32    TypeInfoDataCompare(const TypeInfoData& lhs, const TypeInfoData& rhs) noexcept;
	size_t TypeInfoDataHash(const TypeInfoData& type_info_data) noexcept;

	class TypeInfo {
	  public:
		TypeInfo(const std::type_info& info)
			: m_type_info_data(info.name(), info.raw_name()) {}

		TypeInfo(const TypeInfo& other)
			: m_type_info_data(other.m_type_info_data.undecorated_name, other.m_type_info_data.decorated_name) {}

		TypeInfo& operator=(const TypeInfo&) = delete;

		operator const std::type_info&() const noexcept { return *reinterpret_cast<const std::type_info*>(&m_type_info_data); }
		operator const std::type_info*() const noexcept { return reinterpret_cast<const std::type_info*>(&m_type_info_data); }

		size_t      HashCode() const noexcept { return TypeInfoDataHash(m_type_info_data); }
		bool        operator==(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) == 0; }
		bool        operator!=(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) != 0; }
		bool        Before(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) < 0; }
		bool        operator<(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) < 0; }
		bool        operator>(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) > 0; }
		bool        operator<=(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) <= 0; }
		bool        operator>=(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) >= 0; }
		const char* Name() const noexcept { return m_type_info_data.undecorated_name; }
		const char* RawName() const noexcept { return m_type_info_data.decorated_name; }
		~TypeInfo() noexcept = default;

	  private:
		const TypeInfoData m_type_info_data;
	};

	class TypeIndex {
	  public:
		TypeIndex()
			: m_type_info(nullptr) {}
		TypeIndex(const TypeInfo& info)
			: m_type_info(new TypeInfo(info)) {}
		TypeIndex(const std::type_info& info)
			: m_type_info(new TypeInfo(info)) {}

		TypeIndex(const TypeIndex& other)
			: m_type_info(other.m_type_info ? new TypeInfo(*other.m_type_info) : nullptr) {}
		TypeIndex(TypeIndex&& other) noexcept
			: m_type_info(other.m_type_info) {
			other.m_type_info = nullptr;
		}

		~TypeIndex() { delete m_type_info; }

		size_t          HashCode() const { return m_type_info ? m_type_info->HashCode() : 0; }
		const TypeInfo& Info() const { return *m_type_info; }
		const char*     Name() const { return m_type_info ? m_type_info->Name() : ""; }

		bool operator==(const TypeIndex& other) const { return (m_type_info == other.m_type_info) || (m_type_info && other.m_type_info && m_type_info->HashCode() == other.m_type_info->HashCode()); }
		bool operator!=(const TypeIndex& other) const { return !(*this == other); }
		bool operator<(const TypeIndex& other) const { return (m_type_info && other.m_type_info && m_type_info->HashCode() < other.m_type_info->HashCode()) || (!m_type_info && other.m_type_info); }
		bool operator>(const TypeIndex& other) const { return other < *this; }
		bool operator<=(const TypeIndex& other) const { return !(other < *this); }
		bool operator>=(const TypeIndex& other) const { return !(*this < other); }

		TypeIndex& operator=(const TypeIndex& other) {
			m_type_info = other.m_type_info;
			return *this;
		}

		TypeIndex& operator=(TypeIndex&& other) noexcept {
			m_type_info       = other.m_type_info;
			other.m_type_info = nullptr;
			return *this;
		}

	  private:
		const TypeInfo* m_type_info;
	};
} // namespace Spark

template<> struct std::hash<_SPARK TypeIndex> {
	size_t operator()(const _SPARK TypeIndex& type_index) const { return type_index.HashCode(); }
};

#endif