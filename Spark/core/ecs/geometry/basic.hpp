#ifndef SPARK_BASIC_HPP
#define SPARK_BASIC_HPP

#include "../ecs.hpp"

namespace spark
{
	struct triangle
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices = {
					vertex(math::vec3(-size.x, -size.y, 0.0f)), vertex(math::vec3(size.x, -size.y, 0.0f)),
					vertex(math::vec3(0.0f, size.y, 0.0f)),
			};

			_mesh_manager.create_mesh("__triangle__", vertices);

			entity triangle_entity = _ecs.create_entity(
					mesh_component("__triangle__"),
					material_component(material),
					transform_component(position, rotation, size));

			return triangle_entity;
		}
	};

	struct square
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices = {
					vertex(math::vec3(-size.x, -size.y, 0.0f)), vertex(math::vec3(size.x, -size.y, 0.0f)),
					vertex(math::vec3(size.x, size.y, 0.0f)), vertex(math::vec3(-size.x, size.y, 0.0f)),
			};

			std::vector<u32> indices = {
					0, 1, 2, 2, 3, 0
			};

			_mesh_manager.create_mesh("__square__", vertices, indices);

			entity square_entity = _ecs.create_entity(
					mesh_component("__square__"),
					material_component(material),
					transform_component(position, rotation, size));

			return square_entity;
		}
	};

	struct circle
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (f32 i = 0; i < 360; i++)
			{
				f32 angle = math::radians(i);
				vertices.push_back(vertex(math::vec3(size.x * cos(angle), size.y * sin(angle), 0.0f)));
			}

			_mesh_manager.create_mesh("__circle__", vertices);

			entity circle_entity = _ecs.create_entity(
					mesh_component("__circle__"),
					material_component(material),
					transform_component(position, rotation, size));

			return circle_entity;
		}
	};

	struct line
	{
		static entity create(const math::vec3& start = math::vec3(0.0f), const math::vec3& end = math::vec3(1.0f))
		{
			return create("__default__", start, end);
		}

		static entity create(
				const std::string& material,
				const math::vec3& start = math::vec3(0.0f),
				const math::vec3& end = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices = {
					vertex(start), vertex(end),
			};

			_mesh_manager.create_mesh("__line__", vertices);

			entity line_entity = _ecs.create_entity(
					mesh_component("__line__"), material_component(material));

			return line_entity;
		}
	};

	struct cube
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();


			std::vector<spark::vertex> vertices = {
					spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									size.x, -size.y, -size.z)),
					spark::vertex(spark::math::vec3(size.x, size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									size.x, size.y, -size.z)),
					spark::vertex(spark::math::vec3(-size.x, size.y, -size.z)),
					spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									-size.x, -size.y, size.z)), spark::vertex(
							spark::math::vec3(
									size.x, -size.y, size.z)), spark::vertex(
							spark::math::vec3(size.x, size.y, size.z)), spark::vertex(
							spark::math::vec3(
									size.x, size.y, size.z)), spark::vertex(spark::math::vec3(-size.x, size.y, size.z)),
					spark::vertex(
							spark::math::vec3(-size.x, -size.y, size.z)), spark::vertex(
							spark::math::vec3(
									-size.x, size.y, size.z)),
					spark::vertex(spark::math::vec3(-size.x, size.y, -size.z)), spark::vertex(
							spark::math::vec3(-size.x, -size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									-size.x, -size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									-size.x, -size.y, size.z)),
					spark::vertex(spark::math::vec3(-size.x, size.y, size.z)), spark::vertex(
							spark::math::vec3(
									size.x, size.y, size.z)), spark::vertex(spark::math::vec3(size.x, size.y, -size.z)),
					spark::vertex(
							spark::math::vec3(
									size.x, -size.y, -size.z)),
					spark::vertex(spark::math::vec3(size.x, -size.y, -size.z)),
					spark::vertex(spark::math::vec3(size.x, -size.y, size.z)), spark::vertex(
							spark::math::vec3(
									size.x, size.y, size.z)),
					spark::vertex(spark::math::vec3(-size.x, -size.y, -size.z)),
					spark::vertex(spark::math::vec3(size.x, -size.y, -size.z)),
					spark::vertex(spark::math::vec3(size.x, -size.y, size.z)), spark::vertex(
							spark::math::vec3(
									size.x, -size.y, size.z)),
					spark::vertex(spark::math::vec3(-size.x, -size.y, size.z)), spark::vertex(
							spark::math::vec3(-size.x, -size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									-size.x, size.y, -size.z)), spark::vertex(
							spark::math::vec3(
									size.x, size.y, -size.z)), spark::vertex(spark::math::vec3(size.x, size.y, size.z)),
					spark::vertex(
							spark::math::vec3(
									size.x, size.y, size.z)), spark::vertex(spark::math::vec3(-size.x, size.y, size.z)),
					spark::vertex(
							spark::math::vec3(-size.x, size.y, -size.z))
			};

			_mesh_manager.create_mesh("__cube__", vertices);

			entity cube_entity = _ecs.create_entity(
					mesh_component("__cube__"),
					material_component(material),
					transform_component(position, rotation, size));

			return cube_entity;
		}
	};

	struct sphere
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f),
				const i32 slices = 36,
				const i32 stacks = 18)
		{
			return create("__default__", position, rotation, size, slices, stacks);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f),
				const i32 slices = 36,
				const i32 stacks = 18)
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;
			std::vector<u32> indices;

			// Generate vertices
			for (i32 i = 0; i <= stacks; ++i)
			{
				// phi ranges from 0 to pi
				f32 phi = math::pi<f32>() * f32(i) / stacks;
				for (i32 j = 0; j <= slices; ++j)
				{
					// theta ranges from 0 to 2pi
					f32 theta = 2 * math::pi<f32>() * f32(j) / slices;

					f32 x = size.x * sin(phi) * cos(theta);
					f32 y = size.y * sin(phi) * sin(theta);
					f32 z = size.z * cos(phi);

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			// Generate indices
			for (i32 i = 0; i < stacks; ++i)
			{
				for (i32 j = 0; j < slices; ++j)
				{
					i32 first = (i * (slices + 1)) + j;
					i32 second = first + slices + 1;

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
					transform_component(position, rotation, size));

			return sphere_entity;
		}
	};


	struct capsule
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (f32 i = 0; i < 360; i += 10)
			{
				for (f32 j = 0; j < 360; j += 10)
				{
					f32 x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					f32 y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					f32 z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__capsule__", vertices);

			entity capsule_entity = _ecs.create_entity(
					mesh_component("__capsule__"),
					material_component(material),
					transform_component(position, rotation, size));

			return capsule_entity;
		}
	};

	struct cone
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (f32 i = 0; i < 360; i += 10)
			{
				for (f32 j = 0; j < 360; j += 10)
				{
					f32 x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					f32 y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					f32 z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__cone__", vertices);

			entity cone_entity = _ecs.create_entity(
					mesh_component("__cone__"),
					material_component(material),
					transform_component(position, rotation, size));

			return cone_entity;
		}
	};

	struct cylinder
	{
		static entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = engine::get<ecs>();
			auto& _mesh_manager = engine::get<mesh_manager>();
			auto& _material_manager = engine::get<material_manager>();

			std::vector<vertex> vertices;

			for (f32 i = 0; i < 360; i += 10)
			{
				for (f32 j = 0; j < 360; j += 10)
				{
					f32 x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					f32 y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					f32 z = size.z * cos(math::radians(j));

					vertices.push_back(vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create_mesh("__cylinder__", vertices);

			entity cylinder_entity = _ecs.create_entity(
					mesh_component("__cylinder__"),
					material_component(material),
					transform_component(position, rotation, size));

			return cylinder_entity;
		}
	};

	template <typename T, typename... Args>
	entity create_shape(Args&& ... args)
	{
		return T::create(std::forward<Args>(args)...);
	}
}

#endif