#ifndef SPARK_BASIC_HPP
#define SPARK_BASIC_HPP

#include "../ecs.hpp"

namespace spark
{
	struct triangle
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
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
				transform_component(position, rotation, size)
			);

			return triangle_entity;
		}
	};

	struct square
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
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
				transform_component(position, rotation, size)
			);

			return square_entity;
		}
	};

	struct circle
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (float32_t i = 0; i < 360; i++)
			{
				float32_t angle = math::radians(i);
				vertices.push_back(vertex(math::vec3(size.x * cos(angle), size.y * sin(angle), 0.0f)));
			}

			_mesh_manager.create_mesh("__circle__", vertices);

			entity circle_entity = _ecs.create_entity(
				mesh_component("__circle__"),
				material_component(material),
				transform_component(position, rotation, size)
			);

			return circle_entity;
		}
	};

	struct line
	{
		static entity create(const math::vec3& start = math::vec3(0.0f), const math::vec3& end = math::vec3(1.0f))
		{
			return create("__default__", start, end);
		}

		static entity create(const std::string& material, const math::vec3& start = math::vec3(0.0f), const math::vec3& end = math::vec3(1.0f))
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
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
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
				transform_component(position, rotation, size)
			);

			return cube_entity;
		}
	};

	struct sphere
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f), const int32_t slices = 36, const int32_t stacks = 18)
		{
			return create("__default__", position, rotation, size, slices, stacks);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f), const int32_t slices = 36, const int32_t stacks = 18)
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;
			std::vector<uint32_t> indices;

			// Generate vertices
			for (int32_t i = 0; i <= stacks; ++i)
			{
				// phi ranges from 0 to pi
				float32_t phi = math::pi<float32_t>() * float32_t(i) / stacks;
				for (int32_t j = 0; j <= slices; ++j)
				{
					// theta ranges from 0 to 2pi
					float32_t theta = 2 * math::pi<float32_t>() * float32_t(j) / slices;

					float32_t x = size.x * sin(phi) * cos(theta);
					float32_t y = size.y * sin(phi) * sin(theta);
					float32_t z = size.z * cos(phi);

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			// Generate indices
			for (int32_t i = 0; i < stacks; ++i)
			{
				for (int32_t j = 0; j < slices; ++j)
				{
					int32_t first = (i * (slices + 1)) + j;
					int32_t second = first + slices + 1;

					indices.push_back(first);
					indices.push_back(second);
					indices.push_back(first + 1);

					indices.push_back(second);
					indices.push_back(second + 1);
					indices.push_back(first + 1);
				}
			}

			_mesh_manager.create_mesh("__sphere__", vertices, indices);

			entity sphere_entity = _ecs.create_entity(
				mesh_component("__sphere__"),
				material_component(material),
				transform_component(position, rotation, size)
			);

			return sphere_entity;
		}
	};


	struct capsule
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (float32_t i = 0; i < 360; i += 10)
			{
				for (float32_t j = 0; j < 360; j += 10)
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
				transform_component(position, rotation, size)
			);

			return capsule_entity;
		}
	};

	struct cone
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (float32_t i = 0; i < 360; i += 10)
			{
				for (float32_t j = 0; j < 360; j += 10)
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
				transform_component(position, rotation, size)
			);

			return cone_entity;
		}
	};

	struct cylinder
	{
		static entity create(const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(const std::string& material, const math::vec3& position = math::vec3(0.0f), const math::vec3& rotation = math::vec3(0.0f), const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (float32_t i = 0; i < 360; i += 10)
			{
				for (float32_t j = 0; j < 360; j += 10)
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
				transform_component(position, rotation, size)
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