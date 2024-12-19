#ifndef SPARK_ASSET_HPP
#define SPARK_ASSET_HPP

#include <core/pch.hpp>

namespace Spark {
	template<typename T> class AssetContainer;

	template<typename T> class Asset {
	  public:
		Asset(const String& name)
			: m_name(name) {}
		virtual ~Asset() = default;
		const String& GetName() const { return m_name; }

	  protected:
		// Change this to allow the specific container to access the name
		template<typename U> friend class AssetContainer;

		String m_name;

		// Add a method to set the name if needed
		void SetName(const String& name) { m_name = name; }
	};

	template<typename T> struct IsAnyAsset {
		template<typename U> static constexpr bool Check(const Asset<U>*) { return true; }
		static constexpr bool                      Check(...) { return false; }

		static constexpr bool Value = Check(static_cast<const T*>(nullptr));
	};


	class IAssetContainer {
	  public:
		virtual ~IAssetContainer() = default;
		virtual void Clear()       = 0;
	};

	template<typename T> class AssetContainer : public IAssetContainer {
	  public:
		static_assert(IsAnyAsset<T>::Value, "Type must derive from Asset directly or indirectly");

		template<typename... Args> T& Create(const String& name, Args&&... args) {
			std::lock_guard lock(m_mutex);
			if (auto it = m_assets.Find(name); it != m_assets.End()) {
				LOG_WARN("Asset already exists: " + name);
				return *it->second;
			}
			auto  asset    = MakeUnique<T>(name, Forward<Args>(args)...);
			auto& ref      = *asset;
			m_assets[name] = Move(asset);
			return ref;
		}

		T& Get(const String& name) {
			std::lock_guard lock(m_mutex);
			auto            it = m_assets.Find(name);
			if (it == m_assets.End()) {
				LOG_FATAL("Asset not found: " + name);
			}
			return *it->second;
		}

		bool Exists(const String& name) const {
			std::lock_guard lock(m_mutex);
			return m_assets.Find(name) != m_assets.End();
		}

		void Remove(const String& name) {
			std::lock_guard lock(m_mutex);
			m_assets.Erase(name);
		}

		void Clear() override {
			std::lock_guard lock(m_mutex);
			m_assets.Clear();
		}

		Vector<String> GetKeys() const {
			std::lock_guard lock(m_mutex);
			Vector<String>  keys;
			for (const auto& [key, _] : m_assets) {
				keys.PushBack(key);
			}
			return keys;
		}

	  private:
		UnorderedMap<String, UniquePtr<T>> m_assets;
		mutable std::mutex                 m_mutex;
	};

	class AssetManager {
	  public:
		template<typename T, typename... Args> T& CreateAsset(const String& name, Args&&... args) {
			static_assert(IsAnyAsset<T>::Value, "Type must derive from Asset directly or indirectly");
			return GetContainer<T>().Create(name, Forward<Args>(args)...);
		}

		template<typename T> T& GetAsset(const String& name) {
			static_assert(IsAnyAsset<T>::Value, "Type must derive from Asset directly or indirectly");
			return GetContainer<T>().Get(name);
		}

		template<typename T> bool HasAsset(const String& name) {
			static_assert(IsAnyAsset<T>::Value, "Type must derive from Asset directly or indirectly");
			return GetContainer<T>().Exists(name);
		}

		template<typename T> void RemoveAsset(const String& name) {
			static_assert(IsAnyAsset<T>::Value, "Type must derive from Asset directly or indirectly");
			GetContainer<T>().Remove(name);
		}

	  private:
		template<typename T> AssetContainer<T>& GetContainer() {
			auto it = m_containers.Find(typeid(T));
			if (it == m_containers.End()) {
				auto  container         = MakeUnique<AssetContainer<T>>();
				auto& ref               = *container;
				m_containers[typeid(T)] = Move(container);
				return ref;
			}
			return static_cast<AssetContainer<T>&>(*it->second);
		}

		UnorderedMap<TypeIndex, UniquePtr<IAssetContainer>> m_containers;
	};

} // namespace Spark

#endif