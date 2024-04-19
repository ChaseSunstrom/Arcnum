#ifndef SPARK_SCENE_PARTITION_HPP
#define SPARK_SCENE_PARTITOIN_HPP

#include "../ecs/ecs.hpp"
#include "../ecs/observer.hpp"

namespace spark
{
	class spatial_partition : public virtual observer
	{
	public:
		spatial_partition(spatial_partition* parent = nullptr) : observer() {}
		virtual ~spatial_partition() = default;

		virtual void add_entity(entity e) {}
		virtual void remove_entity(entity e) {}
		virtual void update_entity(entity e) {}

		virtual void on_notify(std::shared_ptr<event> event) {}

		virtual bool entity_is_inside(entity e) const {
			return false; 
		}
		
		virtual std::vector<entity> query(const math::vec3& position, f32 radius) const 
		{
			return std::vector<entity>();
		}
		virtual std::vector<entity> query(const math::vec3& position, f32 radius, std::function<bool(entity)> filter) const 
		{
			return std::vector<entity>();
		}
		virtual std::vector<entity> query(const math::vec3& min, const math::vec3& max) const 
		{
			return std::vector<entity>();
		}
		virtual std::vector<entity> query(const math::vec3& min, const math::vec3& max, std::function<bool(entity)> filter) const 
		{
			return std::vector<entity>();
		}

	protected:
		std::vector<entity> m_entities;
		spatial_partition* m_parent;
	};
}

#endif