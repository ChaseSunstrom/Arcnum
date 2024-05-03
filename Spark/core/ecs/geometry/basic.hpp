#ifndef SPARK_BASIC_HPP
#define SPARK_BASIC_HPP

#include "../ecs.hpp"

namespace spark
{
	struct Triangle
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices = {
					Vertex(math::vec3(-size.x, -size.y, 0.0f)), Vertex(math::vec3(size.x, -size.y, 0.0f)),
					Vertex(math::vec3(0.0f, size.y, 0.0f)),
			};

			_mesh_manager.create("__triangle__", vertices);

			Entity triangle_entity = _ecs.create_entity(
					MeshComponent("__triangle__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return triangle_entity;
		}
	};

	struct Square
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices = {
					Vertex(math::vec3(-size.x, -size.y, 0.0f)), 
					Vertex(math::vec3(size.x, -size.y, 0.0f)),
					Vertex(math::vec3(size.x, size.y, 0.0f)), 
					Vertex(math::vec3(-size.x, size.y, 0.0f)),
			};

			std::vector<u32> indices = {
					0, 1, 2,
					2, 3, 0
			};

			_mesh_manager.create("__square__", vertices, indices);

			Entity square_entity = _ecs.create_entity(
					MeshComponent("__square__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return square_entity;
		}
	};

	struct Circle
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices;

			for (f32 i = 0; i < 360; i++)
			{
				f32 angle = math::radians(i);
				vertices.push_back(Vertex(math::vec3(size.x * cos(angle), size.y * sin(angle), 0.0f)));
			}

			_mesh_manager.create("__circle__", vertices);

			Entity circle_entity = _ecs.create_entity(
					MeshComponent("__circle__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return circle_entity;
		}
	};

	struct Line
	{
		static Entity create(const math::vec3& start = math::vec3(0.0f), const math::vec3& end = math::vec3(1.0f))
		{
			return create("__default__", start, end);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& start = math::vec3(0.0f),
				const math::vec3& end = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices = {
					Vertex(start), Vertex(end),
			};

			_mesh_manager.create("__line__", vertices);

			Entity line_entity = _ecs.create_entity(
					MeshComponent("__line__"), MaterialComponent(material));

			return line_entity;
		}
	};

	struct Cube
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();


			std::vector<spark::Vertex> vertices = {
					spark::Vertex(spark::math::vec3(-size.x, -size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, -size.y, -size.z)),
					spark::Vertex(spark::math::vec3(size.x, size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, size.y, -size.z)),
					spark::Vertex(spark::math::vec3(-size.x, size.y, -size.z)),
					spark::Vertex(spark::math::vec3(-size.x, -size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									-size.x, -size.y, size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, -size.y, size.z)), spark::Vertex(
							spark::math::vec3(size.x, size.y, size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, size.y, size.z)), spark::Vertex(spark::math::vec3(-size.x, size.y, size.z)),
					spark::Vertex(
							spark::math::vec3(-size.x, -size.y, size.z)), spark::Vertex(
							spark::math::vec3(
									-size.x, size.y, size.z)),
					spark::Vertex(spark::math::vec3(-size.x, size.y, -size.z)), spark::Vertex(
							spark::math::vec3(-size.x, -size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									-size.x, -size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									-size.x, -size.y, size.z)),
					spark::Vertex(spark::math::vec3(-size.x, size.y, size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, size.y, size.z)), spark::Vertex(spark::math::vec3(size.x, size.y, -size.z)),
					spark::Vertex(
							spark::math::vec3(
									size.x, -size.y, -size.z)),
					spark::Vertex(spark::math::vec3(size.x, -size.y, -size.z)),
					spark::Vertex(spark::math::vec3(size.x, -size.y, size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, size.y, size.z)),
					spark::Vertex(spark::math::vec3(-size.x, -size.y, -size.z)),
					spark::Vertex(spark::math::vec3(size.x, -size.y, -size.z)),
					spark::Vertex(spark::math::vec3(size.x, -size.y, size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, -size.y, size.z)),
					spark::Vertex(spark::math::vec3(-size.x, -size.y, size.z)), spark::Vertex(
							spark::math::vec3(-size.x, -size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									-size.x, size.y, -size.z)), spark::Vertex(
							spark::math::vec3(
									size.x, size.y, -size.z)), spark::Vertex(spark::math::vec3(size.x, size.y, size.z)),
					spark::Vertex(
							spark::math::vec3(
									size.x, size.y, size.z)), spark::Vertex(spark::math::vec3(-size.x, size.y, size.z)),
					spark::Vertex(
							spark::math::vec3(-size.x, size.y, -size.z))
			};

			_mesh_manager.create("__cube__", vertices);

			Entity cube_entity = _ecs.create_entity(
					MeshComponent("__cube__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return cube_entity;
		}
	};

	struct Sphere
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f),
				const i32 slices = 36,
				const i32 stacks = 18)
		{
			return create("__default__", position, rotation, size, slices, stacks);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f),
				const i32 slices = 36,
				const i32 stacks = 18)
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices;
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

					vertices.push_back(Vertex(math::vec3(x, y, z)));
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

			_mesh_manager.create("__sphere__", vertices, indices);

			Entity sphere_entity = _ecs.create_entity(
					MeshComponent("__sphere__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return sphere_entity;
		}
	};

	struct Capsule
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices;

			for (f32 i = 0; i < 360; i += 10)
			{
				for (f32 j = 0; j < 360; j += 10)
				{
					f32 x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					f32 y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					f32 z = size.z * cos(math::radians(j));

					vertices.push_back(Vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create("__capsule__", vertices);

			Entity capsule_entity = _ecs.create_entity(
					MeshComponent("__capsule__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return capsule_entity;
		}
	};

	struct Cone
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices;

			for (f32 i = 0; i < 360; i += 10)
			{
				for (f32 j = 0; j < 360; j += 10)
				{
					f32 x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					f32 y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					f32 z = size.z * cos(math::radians(j));

					vertices.push_back(Vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create("__cone__", vertices);

			Entity cone_entity = _ecs.create_entity(
					MeshComponent("__cone__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return cone_entity;
		}
	};

	struct Cylinder
	{
		static Entity create(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			return create("__default__", position, rotation, size);
		}

		static Entity create(
				const std::string& material,
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& size = math::vec3(1.0f))
		{
			auto& _ecs = Engine::get<ECS>();
			auto& _mesh_manager = Engine::get<MeshManager>();
			auto& _material_manager = Engine::get<MaterialManager>();

			std::vector<Vertex> vertices;

			for (f32 i = 0; i < 360; i += 10)
			{
				for (f32 j = 0; j < 360; j += 10)
				{
					f32 x = size.x * cos(math::radians(i)) * sin(math::radians(j));
					f32 y = size.y * sin(math::radians(i)) * sin(math::radians(j));
					f32 z = size.z * cos(math::radians(j));

					vertices.push_back(Vertex(math::vec3(x, y, z)));
				}
			}

			_mesh_manager.create("__cylinder__", vertices);

			Entity cylinder_entity = _ecs.create_entity(
					MeshComponent("__cylinder__"),
					MaterialComponent(material),
					TransformComponent(position, rotation, size));

			return cylinder_entity;
		}
	};

	template <typename T, typename... Args>
	Entity create_shape(Args&&... args)
	{
		return T::create(std::forward<Args>(args)...);
	}
}

#endif