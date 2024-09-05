#ifndef SPARK_MODEL_COMPONENT_HPP
#define SPARK_MODEL_COMPONENT_HPP

#include <core/ecs/component.hpp>
#include <core/pch.hpp>
#include <core/render/material.hpp>

namespace Spark {
	struct ModelComponent : public Component {
		ModelComponent(const std::string& model_name, Material* material = nullptr)
			: model_name(model_name)
			, material(material) {}
		std::string model_name;
		Material*   material;
	};
} // namespace Spark

#endif // SPARK_MODEL_COMPONENT_HPP