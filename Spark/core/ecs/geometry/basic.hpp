#ifndef SPARK_BASIC_HPP
#define SPARK_BASIC_HPP

#include "../ecs.hpp"

namespace spark
{
	struct triangle
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position, const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices = {
				vertex(math::vec3(-size.x, -size.y, 0.0f)),
				vertex(math::vec3(size.x, -size.y, 0.0f)),
				vertex(math::vec3(0.0f, size.y, 0.0f)),
			};

			_mesh_manager.create_mesh("__triangle__", vertices);

			entity triangle_entity = _ecs.create_entity(
				mesh_component("__triangle__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return triangle_entity;
		}
	};

	struct square
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices = {
				vertex(math::vec3(-size.x, -size.y, 0.0f)),
				vertex(math::vec3(size.x, -size.y, 0.0f)),
				vertex(math::vec3(size.x, size.y, 0.0f)),
				vertex(math::vec3(-size.x, size.y, 0.0f)),
			};

			_mesh_manager.create_mesh("__square__", vertices);

			entity square_entity = _ecs.create_entity(
				mesh_component("__square__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return square_entity;
		}
	};

	struct circle
	{

	};

	struct rectangle
	{

	};

	struct line
	{

	};

	struct cube
	{

	};

	struct pyramid
	{

	};

	struct sphere
	{

	};

	struct capsule
	{

	};

	struct cone
	{

	};

	struct cylinder
	{

	};

	template <typename T, typename... Args>
	entity create_shape(Args&&... args)
	{
		return T::create(std::forward<Args>(args)...);
	}
}

#endif