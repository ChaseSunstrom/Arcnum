#ifndef SPARK_SERIALIZER_HPP
#define SPARK_SERIALIZER_HPP

#include <core/pch.hpp>

namespace Spark {
	// Inspired by The Cherno's Hazel Engine serialization library
	// https://www.youtube.com/@TheCherno
	class ISerializer {
	  public:
		virtual ~ISerializer()                                = default;

		virtual void WriteData(const char* data, size_t size) = 0;

		void WriteString(const std::string& str) {
			WriteRaw(str.size());
			WriteData((char*) str.c_str(), str.size());
		}

		template<typename T> void WriteRaw(const T& data) { WriteData((char*) &data, sizeof(T)); }

		template<typename T> void WriteObj(const T& data) { T::Serialize(this, data); }

		template<typename K, typename V> void WriteMap(const std::map<K, V>& map) {
			WriteRaw(map.size());
			for (const auto& [key, value] : map) {
				if constexpr (std::is_trivial<K>())
					WriteRaw(key);
				else
					WriteObj(key);

				if constexpr (std::is_trivial<V>())
					WriteRaw(value);
				else
					WriteObj(value);
			}
		}

		template<typename K, typename V> void WriteMap(const std::unordered_map<K, V>& map) {
			WriteRaw(map.size());
			for (const auto& [key, value] : map) {
				if constexpr (std::is_trivial<K>())
					WriteRaw(key);
				else
					WriteObj(key);

				if constexpr (std::is_trivial<V>())
					WriteRaw(value);
				else
					WriteObj(value);
			}
		}

		template<typename V> void WriteMap(const std::unordered_map<std::string, V>& map) {
			WriteRaw(map.size());
			for (const auto& [key, value] : map) {
				WriteString(key);

				if constexpr (std::is_trivial<V>())
					WriteRaw(value);
				else
					WriteObj(value);
			}
		}

		template<typename T> void WriteVector(const std::vector<T>& vec) {
			WriteRaw(vec.size());
			for (const auto& value : vec) {
				if constexpr (std::is_trivial<T>())
					WriteRaw(value);
				else
					WriteObj(value);
			}
		}

		template<typename T> void WriteArray(const T* data, size_t size) {
			WriteRaw(size);
			for (size_t i = 0; i < size; i++) {
				if constexpr (std::is_trivial<T>())
					WriteRaw(data[i]);
				else
					WriteObj(data[i]);
			}
		}
	};

	class IDeserializer {
	  public:
		virtual ~IDeserializer()                       = default;

		virtual void ReadData(char* data, size_t size) = 0;

		void ReadString(std::string& str) {
			size_t size;
			ReadRaw(size);
			str.resize(size);
			char* cstr;
			ReadRaw(cstr);
			str = cstr;
		}

		template<typename T> void ReadRaw(T& data) { ReadData((char*) &data, sizeof(T)); }

		template<typename T> void ReadObj(T& data) { T::Deserialize(this, data); }

		template<typename K, typename V> void ReadMap(std::map<K, V>& map) {
			size_t size;
			ReadRaw(size);
			for (size_t i = 0; i < size; i++) {
				K key;
				if constexpr (std::is_trivial<K>())
					ReadRaw(key);
				else
					ReadObj(key);

				V value;
				if constexpr (std::is_trivial<V>())
					ReadRaw(value);
				else
					ReadObj(value);

				map[key] = value;
			}
		}

		template<typename K, typename V> void ReadMap(std::unordered_map<K, V>& map) {
			size_t size;
			ReadRaw(size);
			for (size_t i = 0; i < size; i++) {
				K key;
				if constexpr (std::is_trivial<K>())
					ReadRaw(key);
				else
					ReadObj(key);

				V value;
				if constexpr (std::is_trivial<V>())
					ReadRaw(value);
				else
					ReadObj(value);

				map[key] = value;
			}
		}

		template<typename V> void ReadMap(std::unordered_map<std::string, V>& map) {
			size_t size;
			ReadRaw(size);
			for (size_t i = 0; i < size; i++) {
				std::string key;
				ReadString(key);

				V value;
				if constexpr (std::is_trivial<V>())
					ReadRaw(value);
				else
					ReadObj(value);

				map[key] = value;
			}
		}

		template<typename T> void ReadVector(std::vector<T>& vec) {
			size_t size;
			ReadRaw(size);
			vec.resize(size);
			for (size_t i = 0; i < size; i++) {
				if constexpr (std::is_trivial<T>())
					ReadRaw(vec[i]);
				else
					ReadObj(vec[i]);
			}
		}

		template<typename T> void ReadArray(T* data, size_t size) {
			size_t arraySize;
			ReadRaw(arraySize);
			for (size_t i = 0; i < arraySize; i++) {
				if constexpr (std::is_trivial<T>())
					ReadRaw(data[i]);
				else
					ReadObj(data[i]);
			}
		}
	};
} // namespace Spark
#endif