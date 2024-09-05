#ifndef SPARK_NAME_COMPONENT_HPP
#define SPARK_NAME_COMPONENT_HPP

#include <core/pch.hpp>
#include <core/ecs/component.hpp>

namespace Spark {
	struct NameComponent : public Component {
		std::string name;
	};
}


#endif