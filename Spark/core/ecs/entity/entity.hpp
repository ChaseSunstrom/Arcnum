#ifndef SPARK_ENTITY_HPP
#define SPARK_ENTITY_HPP

#include "../../spark.hpp"
#include "entity_type.hpp"
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

			uint64_t id = m_recycled_ids.top();
			m_recycled_ids.pop();
			return id;
		}

		uint64_t get_entity_count() const
		{
			return m_next_id;
		}



		void destroy_entity(entity e)
		{
			m_recycled_ids.push(e);
		}
	private:
		entity_manager() = default;

		~entity_manager() = default; 
	private:
		entity m_next_id = 0;

		std::stack <entity> m_recycled_ids = std::stack<entity>();
	};
}

#endif //CORE_ENTITY_H