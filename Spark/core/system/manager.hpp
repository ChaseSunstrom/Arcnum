#ifndef SPARK_MANAGER_HPP
#define SPARK_MANAGER_HPP

#include <core/pch.hpp>
#include "registry.hpp"

namespace Spark {
	struct IManager {
		virtual ~IManager() = default;
	};

	template<typename _Ty> class Manager : public IManager {
	  public:
		Manager()
			: m_registry(MakeUnique<Registry<_Ty>>()) {}
		~Manager() = default;

		template<typename DerivedType, typename... Args> RefPtr<DerivedType> Create(const String& name, Args&&... args) {
			static_assert(IsBaseOfV<_Ty, DerivedType>, "DerivedType must inherit from the manager's base type");
			auto* object = new DerivedType(name, Forward<Args>(args)...);
			return RefCast<DerivedType>(Register(name, UniquePtr<_Ty>(object)));
		}

		RefPtr<_Ty> Register(const String& name, UniquePtr<_Ty> object) { return m_registry->Register(name, Move(object)); }

		RefPtr<_Ty> Get(const String& name) const { return m_registry->Get(name); }

		RefPtr<_Ty> Get(const Handle handle) const { return m_registry->Get(handle); }

		_Ty GetCopy(const String& name) const { return m_registry->GetCopy(name); }

		_Ty GetCopy(const Handle handle) const { return m_registry->GetCopy(handle); }

		void Remove(const String& name) { m_registry->Remove(name); }

		void Remove(const Handle handle) { m_registry->Remove(handle); }

		size_t  GetSize() const { return m_registry->GetSize(); }

		Vector<String> GetKeys() const { return m_registry->GetKeys(); }

		void SetRegistry(UniquePtr<Registry<_Ty>> registry) { m_registry = Move(registry); }

		Registry<_Ty>& GetRegistry() const { return *m_registry; }

	  private:
		UniquePtr<Registry<_Ty>> m_registry;
	};
} // namespace Spark

#endif