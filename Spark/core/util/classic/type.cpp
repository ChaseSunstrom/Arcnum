#include "type.hpp"

namespace Spark {
	i32    TypeInfoDataCompare(const TypeInfoData& lhs, const TypeInfoData& rhs) noexcept { return strcmp(lhs.undecorated_name, rhs.undecorated_name); }
	size_t TypeInfoDataHash(const TypeInfoData& type_info_data) noexcept { return std::hash<const char*>{}(type_info_data.undecorated_name); }


} // namespace Spark