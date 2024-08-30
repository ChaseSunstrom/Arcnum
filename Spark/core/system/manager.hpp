#ifndef SPARK_MANAGER_HPP
#define SPARK_MANAGER_HPP

#include "registry.hpp"
#include <core/pch.hpp>

namespace Spark {
struct IManager {
	virtual ~IManager() = default;
};

template <typename T>
class Manager : public IManager {
  public:
	Manager()
		: m_registry(std::make_unique<Registry<T>>()) {}
	~Manager() = default;

	template <typename... Args>
	T& Create(const std::string& name, Args&&... args) {
		T* object = new T(std::forward<Args>(args)...);
		return Register(name, std::unique_ptr<T>(object));
	}

	Handle Register(const std::string& name, std::unique_ptr<T> object) { return m_registry->Register(name, std::move(object)); }

	T& Get(const std::string& name) const { return m_registry->Get(name); }

	T& Get(const Handle handle) const { return m_registry->Get(handle); }

	T GetCopy(const std::string& name) const { return m_registry->GetCopy(name); }

	T GetCopy(const Handle handle) const { return m_registry->GetCopy(handle); }

	void Remove(const std::string& name) { m_registry->Remove(name); }

	void Remove(const Handle handle) { m_registry->Remove(handle); }

	size_t GetSize() const { return m_registry->GetSize(); }

	std::vector<std::string> GetKeys() const { return m_registry->GetKeys(); }

	void SetRegistry(std::unique_ptr<Registry<T>> registry) { m_registry = std::move(registry); }

	Registry<T>& GetRegistry() const { return *m_registry; }

  private:
	std::unique_ptr<Registry<T>> m_registry;
};
} // namespace Spark

#endif