#include "ecs.hpp"

namespace Spark
{
	Entity& Ecs::MakeEntity()
	{
		// Because Entity's constructor is private
		// could be fixed by having std::unique_ptr<Entity> as a friend class but that is less clean IMO
		// especially because the user would then be able to create Entities through a unique ptr
		// which ruins the whole purpose of this function
		Entity* e = new Entity(m_entities.size());
		m_entities.push_back(std::unique_ptr<Entity>(e));
		return *m_entities.back();
	}

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