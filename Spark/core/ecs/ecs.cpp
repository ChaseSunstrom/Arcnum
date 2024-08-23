#include "ecs.hpp"

namespace Spark
{

	Entity& Ecs::GetEntity(const i64 id) const
	{
		return *m_entities[id];
	}

	i64 Ecs::GetEntityCount() const
	{
		return m_entities.size();
	}

	void Ecs::RemoveEntity(const i64 id)
	{
		m_entities.erase(m_entities.begin() + id);
	}
}