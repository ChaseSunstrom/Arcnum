#include "test.hpp"

#include "../ecs/ecs.hpp"
#include "../scene/octree.hpp"
#include "../util/memory.hpp"

namespace spark
{
	namespace test
	{
        struct octree_fixture
        {
            ECS& _ecs;
            Octree _octree;
            std::vector<Entity> _entities;

            // Setup octree with a center and half-size
            octree_fixture() :
                _ecs(ECS::get()),
                _octree(math::vec3(0.0f), math::vec3(50.0f)) // Large enough for multiple entities
            {
                // Register required components
                _ecs.register_component<Transform>();

                // Create some entities with random positions
                std::default_random_engine generator;
                std::uniform_real_distribution<float> distribution(-49.0f, 49.0f);
                for (i32 i = 0; i < 10; ++i) // Create 10 entities
                {
                    Entity ent = _ecs.create_entity();
                    Transform t;
                    t.m_transform = math::translate(math::mat4(1.0f), math::vec3(distribution(generator), distribution(generator), distribution(generator)));
                    _ecs.add_component(ent, t);

                    _entities.push_back(ent);
                }
            }

            // Cleanup function to remove all entities
            ~octree_fixture()
            {
                for (auto& ent : _entities)
                {
                    _ecs.destroy_entity(ent);
                }
            }
        };

        Entity create_transformed_entity(ECS& instance, const math::vec3& position)
        {
            Entity ent = instance.create_entity();
            Transform t;
            t.m_transform = math::translate(math::mat4(1.0f), position);
            instance.add_component(ent, t);
            return ent;
        }
        // Test adding entities to the octree
        TEST(test_octree_add_entity)
        {
            ECS& instance = ECS::get();
            Octree test_octree(math::vec3(0.0f), math::vec3(50.0f), nullptr);
            std::vector<Entity> entities;

            // Create entities within bounds
            entities.push_back(create_transformed_entity(instance, math::vec3(0.0f, 0.0f, 0.0f)));
            entities.push_back(create_transformed_entity(instance, math::vec3(25.0f, 25.0f, 25.0f)));
            entities.push_back(create_transformed_entity(instance, math::vec3(-25.0f, -25.0f, -25.0f)));

            // Add entities to the octree and check if they are inside
            for (auto& ent : entities)
            {
                test_octree.add_entity(ent);
                bool is_inside = test_octree.entity_is_inside(ent);
                EXPECT_TRUE(is_inside);

                if (!is_inside)
                {
                    auto& _transform = instance.get_component<Transform>(ent);
                    auto position = extract_position(_transform.m_transform);
                    std::cout << "Entity expected to be inside octree is outside. Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
                }

                // Clean up
                instance.destroy_entity(ent);
            }
        }

        

        TEST(test_octree_expansion)
        {
            octree_fixture fixture;

            Entity ent = create_transformed_entity(fixture._ecs, math::vec3(100.0f, 100.0f, 100.0f)); // Position outside the initial bounds
            fixture._octree.add_entity(ent);
            EXPECT_TRUE(fixture._octree.entity_is_inside(ent)); // Entity should now be inside after expansion

            fixture._ecs.destroy_entity(ent); // Clean up
        }

        TEST(test_octree_update_entity)
        {
            octree_fixture fixture;

            Entity ent = fixture._entities.front();
            Transform& t = fixture._ecs.get_component<Transform>(ent);
            t.m_transform = math::translate(math::mat4(1.0f), math::vec3(0.0f)); // Move to the origin
            fixture._ecs.set_component(ent, t);

            fixture._octree.update_entity(ent);
            EXPECT_TRUE(fixture._octree.entity_is_inside(ent)); // Check if the entity is inside after update
        }

        TEST(test_octree_query_with_filter)
        {
            octree_fixture fixture;

            auto filter_func = [&fixture](Entity e) -> bool {
                const auto& t = fixture._ecs.get_component<Transform>(e);
                return extract_position(t.m_transform).x > 0.0f; // Filter for positive x
                };

            auto results = fixture._octree.query(math::vec3(0.0f), 50.0f, filter_func);
            for (auto& ent : results)
            {
                const auto& t = fixture._ecs.get_component<Transform>(ent);
                EXPECT_TRUE(extract_position(t.m_transform).x > 0.0f); // Verify filter effectiveness
            }
        }

        TEST(test_octree_subdivision)
        {
            octree_fixture fixture;

            // Ensure subdivision occurs when adding entities beyond the threshold
            for (i32 i = 0; i < 20; ++i)
            {
                Entity ent = create_transformed_entity(fixture._ecs, math::vec3(0.0f)); // All entities at the origin
                fixture._octree.add_entity(ent);
            }

            EXPECT_FALSE(fixture._octree.is_leaf()); // Octree should not be a leaf after subdivision
        }

		bool core_test_main()
		{
			try
			{
				for (const auto& test: TestRegistry::get_tests())
				{
					test.function();

					SPARK_INFO("[CORE TEST PASSED]: " << test.name);
				}
			}
			catch (const std::exception& e)
			{
				SPARK_ERROR("[CORE TEST FAILED]: " << e.what());
				return false;
			}
			return true;
		}
	}
}