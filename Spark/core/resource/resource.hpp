#ifndef SPARK_RESOURCE_HPP
#define SPARK_RESOURCE_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include <core/render/mesh.hpp>
#include <core/render/material.hpp>
#include <core/render/model.hpp>
#include <core/render/shader.hpp>
#include <core/scene/scene.hpp>

namespace Spark
{
	// Dummy class for all resources, which includes, meshes, materials, models, shaders, etc.
	class Resource
	{
	public:
		virtual ~Resource() = default;
	};

	// Overload for Manager<Resource>
	template <>
	struct Manager<Resource>
	{
		Manager<StaticMesh> static_mesh_manager;
		Manager<DynamicMesh> dynamic_mesh_manager;
		Manager<Material> material_manager;
		Manager<StaticModel> static_model_manager;
		Manager<DynamicModel> dynamic_model_manager;
		Manager<Scene> scene_manager;
		Manager<Shader> shader_manager;
	};
}

#endif // SPARK_RESOURCE_HPP