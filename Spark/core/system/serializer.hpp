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

		template<typename _Ty> void WriteRaw(const _Ty& data) { WriteData((char*) &data, sizeof(_Ty)); }

		template<typename _Ty> void WriteObj(const _Ty& data) { _Ty::Serialize(this, data); }

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

		template<typename _Ty> void WriteVector(const std::vector<_Ty>& vec) {
			WriteRaw(vec.size());
			for (const auto& value : vec) {
				if constexpr (std::is_trivial<_Ty>())
					WriteRaw(value);
				else
					WriteObj(value);
			}
		}

		template<typename _Ty> void WriteArray(const _Ty* data, size_t size) {
			WriteRaw(size);
			for (size_t i = 0; i < size; i++) {
				if constexpr (std::is_trivial<_Ty>())
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

		template<typename _Ty> void ReadRaw(_Ty& data) { ReadData((char*) &data, sizeof(_Ty)); }

		template<typename _Ty> void ReadObj(_Ty& data) { _Ty::Deserialize(this, data); }

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

		template<typename _Ty> void ReadVector(std::vector<_Ty>& vec) {
			size_t size;
			ReadRaw(size);
			vec.resize(size);
			for (size_t i = 0; i < size; i++) {
				if constexpr (std::is_trivial<_Ty>())
					ReadRaw(vec[i]);
				else
					ReadObj(vec[i]);
			}
		}

		template<typename _Ty> void ReadArray(_Ty* data, size_t size) {
			size_t arraySize;
			ReadRaw(arraySize);
			for (size_t i = 0; i < arraySize; i++) {
				if constexpr (std::is_trivial<_Ty>())
					ReadRaw(data[i]);
				else
					ReadObj(data[i]);
			}
		}
	};
} // namespace Spark
#endif