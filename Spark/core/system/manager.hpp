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
			: m_registry(std::make_unique<Registry<_Ty>>()) {}
		~Manager() = default;

		template<typename... Args> RefPtr<_Ty> Create(const std::string& name, Args&&... args) {
			_Ty* object = new _Ty(std::forward<Args>(args)...);
			return Register(name, std::unique_ptr<_Ty>(object));
		}

		RefPtr<_Ty> Register(const std::string& name, std::unique_ptr<_Ty> object) { return m_registry->Register(name, std::move(object)); }

		RefPtr<_Ty> Get(const std::string& name) const { return m_registry->Get(name); }

		RefPtr<_Ty> Get(const Handle handle) const { return m_registry->Get(handle); }

		_Ty GetCopy(const std::string& name) const { return m_registry->GetCopy(name); }

		_Ty GetCopy(const Handle handle) const { return m_registry->GetCopy(handle); }

		void Remove(const std::string& name) { m_registry->Remove(name); }

		void Remove(const Handle handle) { m_registry->Remove(handle); }

		size_t GetSize() const { return m_registry->GetSize(); }

		std::vector<std::string> GetKeys() const { return m_registry->GetKeys(); }

		void SetRegistry(std::unique_ptr<Registry<_Ty>> registry) { m_registry = std::move(registry); }

		Registry<_Ty>& GetRegistry() const { return *m_registry; }

	  private:
		std::unique_ptr<Registry<_Ty>> m_registry;
	};
} // namespace Spark

#endif