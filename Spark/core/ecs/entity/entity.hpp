#ifndef SPARK_ENTITY_HPP
#define SPARK_ENTITY_HPP

#include "../../spark.hpp"
#include "entity_type.hpp"
#include "../../net/serializeable.hpp"
#include "../component/component.hpp"

namespace spark
{
	class entity_manager
	{
	public:
		static entity_manager& get()
		{
			static entity_manager instance;
			return instance;
		}
		entity create_entity()
		{
			if (m_recycled_ids.empty())
			{
				return m_next_id++;
			}

			uint64_t id = m_recycled_ids.front();
			m_recycled_ids.pop_front();
			return id;
		}

		uint64_t get_entity_count() const
		{
			return m_next_id;
		}

		void destroy_entity(entity e)
		{
			m_recycled_ids.push_front(e);
		}
	private:
		entity_manager() = default;

		~entity_manager() = default; 
	private:
		entity m_next_id = 0;

		std::deque <entity> m_recycled_ids;
		SERIALIZE_MEMBERS(entity_manager, m_next_id, m_recycled_ids)
	};
}

#endif //CORE_ENTITY_H