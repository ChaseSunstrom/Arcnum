#ifndef SPARK_REGISTRY_HPP
#define SPARK_REGISTRY_HPP

#include <core/pch.hpp>
#include <core/util/memory/ref_ptr.hpp>

namespace Spark {
	struct Handle {
		size_t index;
		u32    generation;
		bool   operator==(const Handle& other) const { return index == other.index && generation == other.generation; }
	};

	// Basically just a wrapper around a std::unordered_map<std::string,
	// std::unique_ptr<_Ty>> so we can name the elements put into this (very useful
	// for keeping track of meshes, materials, shaders, etc.)
	template<typename _Ty> class Registry {
	public:
		Registry()                           = default;
		~Registry()                          = default;
		Registry(const Registry&)            = delete;
		Registry& operator=(const Registry&) = delete;

		RefPtr<_Ty> Register(const std::string& name, std::unique_ptr<_Ty> object) {
			Handle handle;
			if (!m_free_indices.empty()) {
				handle.index = m_free_indices.back();
				m_free_indices.pop_back();
				handle.generation = m_generations[handle.index]++;
			} else {
				handle.index      = m_handle_values.size();
				handle.generation = 0;
				m_handle_values.emplace_back(std::nullopt);
				m_generations.push_back(0);
			}

			_Ty& ref                        = *object;
			m_handle_values[handle.index] = std::ref(ref);
			m_registry[name]              = std::make_pair(handle, std::move(object));
			return ref;
		}

		void Remove(const std::string& name) {
			auto it = m_registry.find(name);
			if (it != m_registry.end()) {
				Handle handle = it->second.first;
				m_free_indices.push_back(handle.index);
				m_handle_values[handle.index] = std::nullopt;
				m_generations[handle.index]++; // Invalidate existing handles
				m_registry.erase(it);
			}
		}

		void Remove(const Handle handle) {
			if (handle.index < m_handle_values.size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value()) {
				m_free_indices.push_back(handle.index);
				m_handle_values[handle.index] = std::nullopt;
				m_generations[handle.index]++; // Invalidate existing handles
				for (auto& [key, value] : m_registry) {
					if (value.first == handle) {
						m_registry.erase(key);
						break;
					}
				}
			}
		}

		RefPtr<_Ty> Get(const std::string& name) const {
			auto it = m_registry.find(name);
			if (it != m_registry.end())
				return *it->second.second;

			LOG_FATAL("Could not find object with name: " << name);
		}

		size_t GetSize() const { return m_registry.size(); }

		RefPtr<_Ty> Get(const Handle handle) const {
			if (handle.index < m_handle_values.size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value()) {
				return m_handle_values[handle.index].value().get();
			}
			LOG_FATAL("Invalid handle: index " << handle.index << ", generation " << handle.generation);
		}

		bool IsHandleValid(const Handle& handle) const {
			return handle.index < m_handle_values.size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value();
		}

		_Ty GetCopy(const std::string& name) const {
			auto it = m_registry.find(name);
			if (it != m_registry.end())
				return *it->second;
			LOG_FATAL("Could not find object with name: " << name);
		}

		_Ty GetCopy(const Handle handle) const {
			if (handle.index < m_handle_values.size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value()) {
				return m_handle_values[handle.index].value().get();
			}
			LOG_FATAL("Invalid handle: index " << handle.index << ", generation " << handle.generation);
		}

		std::vector<std::string> GetKeys() const {
			std::vector<std::string> keys;
			for (auto& [key, value] : m_registry)
				keys.push_back(key);
			return keys;
		}

		std::vector<std::reference_wrapper<_Ty>>& GetValues() const { return m_handle_values; }

		bool Contains(const std::string& name) const { return m_registry.find(name) != m_registry.end(); }

		bool Contains(const Handle handle) const { return handle.index < m_handle_values.size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value(); }

		void Clear() { m_registry.clear(); }

		u32 Size() const { return m_registry.size(); }

	private:
		std::unordered_map<std::string, std::pair<Handle, std::unique_ptr<_Ty>>> m_registry;
		std::vector<std::optional<std::reference_wrapper<_Ty>>>                  m_handle_values;
		std::vector<u32>                                                       m_generations;
		std::vector<u32>                                                       m_free_indices;
	};
} // namespace Spark

#endif