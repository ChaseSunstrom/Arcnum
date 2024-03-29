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
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (int32_t i = 0; i < 360; i++)
			{
				float32_t angle = math::radians(i);
				vertices.push_back(vertex(math::vec3(size.x * cos(angle), size.y * sin(angle), 0.0f)));
			}

			_mesh_manager.create_mesh("__circle__", vertices);

			entity circle_entity = _ecs.create_entity(
				mesh_component("__circle__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return circle_entity;
		}
	};

	struct rectangle
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

			_mesh_manager.create_mesh("__rectangle__", vertices);

			entity rectangle_entity = _ecs.create_entity(
				mesh_component("__rectangle__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return rectangle_entity;
		}
	};

	struct line
	{
		static entity create(const std::string& material = "__default__", const math::vec3& start = math::vec3(0.0f), const math::vec3& end = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices = {
				vertex(start),
				vertex(end),
			};

			_mesh_manager.create_mesh("__line__", vertices);

			entity line_entity = _ecs.create_entity(
				mesh_component("__line__"),
				material_component(material)
			);

			return line_entity;
		}
	};

	struct cube
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();


			std::vector<spark::vertex> vertices = {
				spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(-size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),

				spark::vertex(spark::math::vec3(-size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y,  size.z)),

				spark::vertex(spark::math::vec3(-size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x,  size.y,  size.z)),

				spark::vertex(spark::math::vec3(size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y,  size.z)),

				spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),

				spark::vertex(spark::math::vec3(-size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y, -size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x,  size.y,  size.z)),
				spark::vertex(spark::math::vec3(-size.x,  size.y, -size.z))
			};

			_mesh_manager.create_mesh("__cube__", vertices);

			entity cube_entity = _ecs.create_entity(
				mesh_component("__cube__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return cube_entity;
		}
	};

	struct sphere
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (int32_t i = 0; i < 360; i += 10)
			{
				for (int32_t j = 0; j < 360; j += 10)
				{
					float32_t x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					float32_t y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					float32_t z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__sphere__", vertices);

			entity sphere_entity = _ecs.create_entity(
				mesh_component("__sphere__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return sphere_entity;
		}
	};

	struct capsule
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (int32_t i = 0; i < 360; i += 10)
			{
				for (int32_t j = 0; j < 360; j += 10)
				{
					float32_t x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					float32_t y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					float32_t z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__capsule__", vertices);

			entity capsule_entity = _ecs.create_entity(
				mesh_component("__capsule__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return capsule_entity;
		}
	};

	struct cone
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (int32_t i = 0; i < 360; i += 10)
			{
				for (int32_t j = 0; j < 360; j += 10)
				{
					float32_t x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					float32_t y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					float32_t z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__cone__", vertices);

			entity cone_entity = _ecs.create_entity(
				mesh_component("__cone__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return cone_entity;
		}
	};

	struct cylinder
	{
		static entity create(const std::string& material = "__default__", const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (int32_t i = 0; i < 360; i += 10)
			{
				for (int32_t j = 0; j < 360; j += 10)
				{
					float32_t x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					float32_t y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					float32_t z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__cylinder__", vertices);

			entity cylinder_entity = _ecs.create_entity(
				mesh_component("__cylinder__"),
				material_component(material),
				transform_component(position, rotation)
			);

			return cylinder_entity;
		}
	};

	template <typename T, typename... Args>
	entity create_shape(Args&&... args)
	{
		return T::create(std::forward<Args>(args)...);
	}
}

#endif