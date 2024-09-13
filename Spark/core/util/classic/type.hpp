#ifndef SPARK_TYPE_HPP
#define SPARK_TYPE_HPP

#include <core/util/types.hpp>
#include <typeindex>
#include <typeinfo>

namespace Spark {
	struct TypeInfoData {
		const char* undecorated_name;
		const char  decorated_name[1];

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
		TypeInfo(const TypeInfo&)            = delete;
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
		mutable TypeInfoData m_type_info_data;
	};

	class TypeIndex {
	  public:
		TypeIndex()
			: m_type_info(nullptr) {}
		TypeIndex(const TypeInfo& info)
			: m_type_info(&info) {}
		TypeIndex(const TypeIndex& other)
			: m_type_info(other.m_type_info) {}
		TypeIndex(TypeIndex&& other) noexcept
			: m_type_info(other.m_type_info) {}

		size_t          HashCode() const { return m_type_info->HashCode(); }
		const TypeInfo& Info() const { return *reinterpret_cast<const TypeInfo*>(m_type_info); }
		const char*     Name() const { return m_type_info->Name(); }

		bool operator==(const TypeIndex& other) const { return m_type_info->HashCode() == other.m_type_info->HashCode(); }
		bool operator!=(const TypeIndex& other) const { return m_type_info->HashCode() != other.m_type_info->HashCode(); }
		bool operator<(const TypeIndex& other) const { return m_type_info->HashCode() < other.m_type_info->HashCode(); }
		bool operator>(const TypeIndex& other) const { return m_type_info->HashCode() > other.m_type_info->HashCode(); }
		bool operator<=(const TypeIndex& other) const { return m_type_info->HashCode() <= other.m_type_info->HashCode(); }
		bool operator>=(const TypeIndex& other) const { return m_type_info->HashCode() >= other.m_type_info->HashCode(); }

		TypeIndex& operator=(const TypeIndex& other) {
			m_type_info = other.m_type_info;
			return *this;
		}

		TypeIndex& operator=(TypeIndex&& other) noexcept {
			m_type_info = other.m_type_info;
			return *this;
		}

	  private:
		const TypeInfo* m_type_info;
	};
} // namespace Spark

template<> struct std::hash<Spark::TypeIndex> {
	size_t operator()(const Spark::TypeIndex& type_index) const { return type_index.HashCode(); }
};

#endif