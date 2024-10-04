#ifndef SPARK_TYPE_HPP
#define SPARK_TYPE_HPP

#include <core/util/types.hpp>
#include <core/util/defines.hpp>
#include <core/util/types.hpp>
#include <typeinfo>
#include "hash.hpp"

namespace Spark {
    /**
     * @brief Struct to hold type information data.
     */
    struct TypeInfoData {
        const char* undecorated_name;
        const char* decorated_name;

        TypeInfoData(const char* undecorated, const char* decorated)
            : undecorated_name(undecorated)
            , decorated_name(decorated) {}

        // Disable default constructor and copy/move operations
        TypeInfoData() = delete;
        TypeInfoData(const TypeInfoData&) = delete;
        TypeInfoData(TypeInfoData&&) = delete;
        TypeInfoData& operator=(const TypeInfoData&) = delete;
        TypeInfoData& operator=(TypeInfoData&&) = delete;
    };

    /**
     * @brief Compare two TypeInfoData objects.
     * @param lhs Left-hand side TypeInfoData.
     * @param rhs Right-hand side TypeInfoData.
     * @return Integer result of comparison.
     */
    i32 TypeInfoDataCompare(const TypeInfoData& lhs, const TypeInfoData& rhs) noexcept;

    /**
     * @brief Hash a TypeInfoData object.
     * @param type_info_data TypeInfoData to hash.
     * @return Hash value.
     */
    size_t TypeInfoDataHash(const TypeInfoData& type_info_data) noexcept;

    /**
     * @brief Class to represent type information.
     */
    class TypeInfo {
    public:
        /**
         * @brief Construct a TypeInfo from std::type_info.
         * @param info Reference to std::type_info.
         */
        TypeInfo(const std::type_info& info)
            : m_type_info_data(info.name(), info.raw_name()) {}

        /**
         * @brief Copy constructor.
         * @param other TypeInfo to copy from.
         */
        TypeInfo(const TypeInfo& other)
            : m_type_info_data(other.m_type_info_data.undecorated_name, other.m_type_info_data.decorated_name) {}

        // Disable copy assignment
        TypeInfo& operator=(const TypeInfo&) = delete;

        /**
         * @brief Convert to std::type_info reference.
         */
        operator const std::type_info&() const noexcept { return *reinterpret_cast<const std::type_info*>(&m_type_info_data); }

        /**
         * @brief Convert to std::type_info pointer.
         */
        operator const std::type_info*() const noexcept { return reinterpret_cast<const std::type_info*>(&m_type_info_data); }

        /**
         * @brief Get hash code of the type.
         * @return Hash code.
         */
        size_t HashCode() const noexcept { return TypeInfoDataHash(m_type_info_data); }

        /**
         * @brief Comparison operators.
         */
        bool operator==(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) == 0; }
        bool operator!=(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) != 0; }
        bool Before(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) < 0; }
        bool operator<(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) < 0; }
        bool operator>(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) > 0; }
        bool operator<=(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) <= 0; }
        bool operator>=(const TypeInfo& other) const noexcept { return TypeInfoDataCompare(m_type_info_data, other.m_type_info_data) >= 0; }

        /**
         * @brief Get undecorated name of the type.
         * @return Undecorated name.
         */
        const char* Name() const noexcept { return m_type_info_data.undecorated_name; }

        /**
         * @brief Get decorated name of the type.
         * @return Decorated name.
         */
        const char* RawName() const noexcept { return m_type_info_data.decorated_name; }

        /**
         * @brief Default destructor.
         */
        ~TypeInfo() noexcept = default;

    private:
        const TypeInfoData m_type_info_data;
    };

    /**
     * @brief Class to represent a type index.
     */
    class TypeIndex {
    public:
        /**
         * @brief Default constructor.
         */
        TypeIndex() : m_type_info(nullptr) {}

        /**
         * @brief Construct from TypeInfo.
         * @param info Reference to TypeInfo.
         */
        TypeIndex(const TypeInfo& info) : m_type_info(new TypeInfo(info)) {}

        /**
         * @brief Construct from std::type_info.
         * @param info Reference to std::type_info.
         */
        TypeIndex(const std::type_info& info) : m_type_info(new TypeInfo(info)) {}

        /**
         * @brief Copy constructor.
         * @param other TypeIndex to copy from.
         */
        TypeIndex(const TypeIndex& other)
            : m_type_info(other.m_type_info ? new TypeInfo(*other.m_type_info) : nullptr) {}

        /**
         * @brief Move constructor.
         * @param other TypeIndex to move from.
         */
        TypeIndex(TypeIndex&& other) noexcept : m_type_info(other.m_type_info) {
            other.m_type_info = nullptr;
        }

        /**
         * @brief Destructor.
         */
        ~TypeIndex() { delete m_type_info; }

        /**
         * @brief Get hash code of the type.
         * @return Hash code.
         */
        size_t HashCode() const { return m_type_info ? m_type_info->HashCode() : 0; }

        /**
         * @brief Get TypeInfo.
         * @return Reference to TypeInfo.
         */
        const TypeInfo& Info() const { return *m_type_info; }

        /**
         * @brief Get name of the type.
         * @return Name of the type.
         */
        const char* Name() const { return m_type_info ? m_type_info->Name() : ""; }

        /**
         * @brief Comparison operators.
         */
        bool operator==(const TypeIndex& other) const { return (m_type_info == other.m_type_info) || (m_type_info && other.m_type_info && m_type_info->HashCode() == other.m_type_info->HashCode()); }
        bool operator!=(const TypeIndex& other) const { return !(*this == other); }
        bool operator<(const TypeIndex& other) const { return (m_type_info && other.m_type_info && m_type_info->HashCode() < other.m_type_info->HashCode()) || (!m_type_info && other.m_type_info); }
        bool operator>(const TypeIndex& other) const { return other < *this; }
        bool operator<=(const TypeIndex& other) const { return !(other < *this); }
        bool operator>=(const TypeIndex& other) const { return !(*this < other); }

        /**
         * @brief Copy assignment operator.
         * @param other TypeIndex to copy from.
         * @return Reference to this TypeIndex.
         */
        TypeIndex& operator=(const TypeIndex& other) {
            m_type_info = other.m_type_info;
            return *this;
        }

        /**
         * @brief Move assignment operator.
         * @param other TypeIndex to move from.
         * @return Reference to this TypeIndex.
         */
        TypeIndex& operator=(TypeIndex&& other) noexcept {
            m_type_info = other.m_type_info;
            other.m_type_info = nullptr;
            return *this;
        }

    private:
        const TypeInfo* m_type_info;
    };

    template <> struct Hash<TypeIndex> {
		usize operator()(const TypeIndex& type_index) const { return type_index.HashCode(); }
    };
} // namespace Spark

namespace std {
	template<> struct hash<_SPARK TypeIndex> {
		size_t operator()(const _SPARK TypeIndex& type_index) const { return type_index.HashCode(); }
	};
} // namespace std

#endif