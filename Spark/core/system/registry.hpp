#ifndef SPARK_REGISTRY_HPP
#define SPARK_REGISTRY_HPP

#include <core/pch.hpp>
#include <core/util/memory/ref_ptr.hpp>

namespace Spark {
	struct Handle {
		size_t  index;
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

		RefPtr<_Ty> Register(const String& name, UniquePtr<_Ty> object) {
			Handle handle;
			if (!m_free_indices.Empty()) {
				handle.index = m_free_indices.Back();
				m_free_indices.PopBack();
				handle.generation = m_generations[handle.index]++;
			} else {
				handle.index      = m_handle_values.Size();
				handle.generation = 0;
				m_handle_values.EmplaceBack(std::nullopt);
				m_generations.PushBack(0);
			}

			_Ty& ref                        = *object;
			m_handle_values[handle.index] = std::ref(ref);
			m_registry[name]              = Pair<Handle, UniquePtr<_Ty>>(handle, std::move(object));
			return ref;
		}

		void Remove(const String& name) {
			auto it = m_registry.Find(name);
			if (it != m_registry.End()) {
				Handle handle = it->second.first;
				m_free_indices.PushBack(handle.index);
				m_handle_values[handle.index] = std::nullopt;
				m_generations[handle.index]++; // Invalidate existing handles
				m_registry.Erase(it);
			}
		}

		void Remove(const Handle handle) {
			if (handle.index < m_handle_values.Size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value()) {
				m_free_indices.PushBack(handle.index);
				m_handle_values[handle.index] = std::nullopt;
				m_generations[handle.index]++; // Invalidate existing handles
				for (auto& [key, value] : m_registry) {
					if (value.first == handle) {
						m_registry.Erase(key);
						break;
					}
				}
			}
		}

		RefPtr<_Ty> Get(const String& name) const {
			auto it = m_registry.Find(name);
			if (it != m_registry.End())
				return *it->second.second;

			LOG_FATAL("Could not find object with name: " << name);
		}

		size_t GetSize() const { return m_registry.Size(); }

		RefPtr<_Ty> Get(const Handle handle) const {
			if (handle.index < m_handle_values.Size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value()) {
				return m_handle_values[handle.index].value().get();
			}
			LOG_FATAL("Invalid handle: index " << handle.index << ", generation " << handle.generation);
		}

		bool IsHandleValid(const Handle& handle) const {
			return handle.index < m_handle_values.Size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value();
		}

		_Ty GetCopy(const String& name) const {
			auto it = m_registry.Find(name);
			if (it != m_registry.End())
				return *it->second;
			LOG_FATAL("Could not find object with name: " << name);
		}

		_Ty GetCopy(const Handle handle) const {
			if (handle.index < m_handle_values.Size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value()) {
				return m_handle_values[handle.index].value().get();
			}
			LOG_FATAL("Invalid handle: index " << handle.index << ", generation " << handle.generation);
		}

		Vector<String> GetKeys() const {
			Vector<String> keys;
			for (auto& [key, value] : m_registry)
				keys.PushBack(key);
			return keys;
		}

		Vector<std::reference_wrapper<_Ty>>& GetValues() const { return m_handle_values; }

		bool Contains(const String& name) const { return m_registry.Find(name) != m_registry.End(); }

		bool Contains(const Handle handle) const { return handle.index < m_handle_values.Size() && handle.generation == m_generations[handle.index] && m_handle_values[handle.index].has_value(); }

		void Clear() { m_registry.Clear(); }

		u32 Size() const { return m_registry.Size(); }

	private:
		UnorderedMap<String, Pair<Handle, UniquePtr<_Ty>>> m_registry;
		Vector<std::optional<std::reference_wrapper<_Ty>>> m_handle_values;
		Vector<u32>                                        m_generations;
		Vector<u32>                                        m_free_indices;
	};
} // namespace Spark

#endif