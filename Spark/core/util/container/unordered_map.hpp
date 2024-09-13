#ifndef SPARK_UNORDERED_MAP_HPP
#define SPARK_UNORDERED_MAP_HPP

#include <core/util/types.hpp>
#include <initializer_list>
#include <unordered_map>
#include "list.hpp"
#include "pair.hpp"
#include "vector.hpp"

namespace Spark {
	template<typename Key, typename Value, typename Hash = std::hash<Key>> class UnorderedMap {
	  public:
		struct Node {
			Pair<const Key, Value> m_pair;
			Node(const Key& key, const Value& value)
				: m_pair(key, value) {}
			~Node() = default;
		};

        using Bucket = List<Node>;
        using ValueType = Pair<const Key, Value>;
        using Pointer = ValueType*;
        using Reference = ValueType&;

        class Iterator {
		  public:
			Iterator(Vector<Bucket>* buckets, size_t bucket_index)
				: m_buckets(buckets)
				, m_bucket_index(bucket_index)
				, m_it(bucket_index < buckets->Size() ? (*buckets)[bucket_index].Begin() : nullptr) {
				if (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
					++(*this);
				}
			}

			Iterator(Vector<Bucket>* buckets, size_t bucket_index, typename Bucket::Iterator it)
				: m_buckets(buckets)
				, m_bucket_index(bucket_index)
				, m_it(it) {}

			Reference operator*() const { return (*m_it).m_pair; }
			Pointer   operator->() const { return &((*m_it).m_pair); }

			Iterator& operator++() {
				if (m_bucket_index >= m_buckets->Size()) {
					return *this;
				}

				++m_it;
				while (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
					++m_bucket_index;
					if (m_bucket_index < m_buckets->Size()) {
						m_it = (*m_buckets)[m_bucket_index].Begin();
					}
				}

				return *this;
			}

			Iterator operator++(i32) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const Iterator& other) const {
				return m_buckets == other.m_buckets && m_bucket_index == other.m_bucket_index && (m_bucket_index == m_buckets->Size() || m_it == other.m_it);
			}

			bool operator!=(const Iterator& other) const { return !(*this == other); }

		  private:
			Vector<Bucket>*           m_buckets;
			size_t                    m_bucket_index;
			typename Bucket::Iterator m_it;
		};


        using ConstIterator = Iterator;

		UnorderedMap()
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {}

		UnorderedMap(size_t bucket_count)
			: m_buckets(bucket_count)
			, m_size(0) {}

		UnorderedMap(const UnorderedMap& other)
			: m_buckets(other.m_buckets)
			, m_size(other.m_size)
			, m_hasher(other.m_hasher) {}

		UnorderedMap(UnorderedMap&& other) noexcept
			: m_buckets(Move(other.m_buckets))
			, m_size(other.m_size)
			, m_hasher(Move(other.m_hasher)) {
			other.m_size = 0;
		}

		UnorderedMap(const std::unordered_map<Key, Value>& map)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
			for (const auto& pair : map) {
				Insert(pair);
			}
		}

		UnorderedMap(const std::initializer_list<Pair<Key, Value>>& list)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
			for (const auto& pair : list) {
				Insert(pair);
			}
		}

		~UnorderedMap() { Clear(); }

		UnorderedMap& operator=(const UnorderedMap& other) {
			if (this != &other) {
				Clear();
				m_buckets = other.m_buckets;
				m_size    = other.m_size;
				m_hasher  = other.m_hasher;
			}
			return *this;
		}

		UnorderedMap& operator=(UnorderedMap&& other) noexcept {
			if (this != &other) {
				Clear();
				m_buckets    = Move(other.m_buckets);
				m_size       = other.m_size;
				m_hasher     = Move(other.m_hasher);
				other.m_size = 0;
			}
			return *this;
		}

		void Insert(const Key& key, const Value& value) {
			if (static_cast<f32>(m_size + 1) / m_buckets.Size() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			size_t index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (node.m_pair.first == key) {
					node.m_pair.second = value;
					return;
				}
			}

			m_buckets[index].EmplaceBack(key, value);
			++m_size;
		}

		template<typename K, typename V> void Insert(K&& key, V&& value) {
			if (static_cast<f32>(m_size + 1) / m_buckets.Size() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			size_t index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (node.m_pair.first == key) {
					node.m_pair.second = Forward<V>(value);
					return;
				}
			}

			m_buckets[index].EmplaceBack(Forward<K>(key), Forward<V>(value));
			++m_size;
		}

		template<typename P> void Insert(P&& pair) { Insert(Forward<typename P::FirstType>(pair.first), Forward<typename P::SecondType>(pair.second)); }

		bool Remove(const Key& key) {
			size_t index  = GetBucketIndex(key);
			auto&  bucket = m_buckets[index];
			for (auto it = bucket.Begin(); it != bucket.End(); ++it) {
				if (it->m_pair.first == key) {
					bucket.Erase(it);
					--m_size;
					return true;
				}
			}
			return false;
		}

		Value& At(const Key& key) {
			size_t index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (node.m_pair.first == key) {
					return node.m_pair.second;
				}
			}
			throw std::out_of_range("Key not found");
		}

		const Value& At(const Key& key) const { return const_cast<UnorderedMap*>(this)->At(key); }

		Value& operator[](const Key& key) {
			if (key == std::type_index(typeid(void))) {
				LOG_FATAL("Invalid type_index key");
			}

			size_t index = GetBucketIndex(key);
			std::cout << "Accessing bucket " << index << " for key " << key.name() << std::endl;
			std::cout << "Bucket size: " << m_buckets[index].Size() << std::endl;
			for (auto& node : m_buckets[index]) {
				if (node.m_pair.first == key) {
					return node.m_pair.second;
				}
			}

			m_buckets[index].EmplaceBack(key, Value());
			++m_size;
			return m_buckets[index].Back().m_pair.second;
		}

		bool Contains(const Key& key) const {
			size_t index = GetBucketIndex(key);
			for (const auto& node : m_buckets[index]) {
				if (node.m_pair.first == key) {
					return true;
				}
			}
			return false;
		}

		friend std::ostream& operator<<(std::ostream& os, const UnorderedMap& map) {
			os << "{ ";
			for (size_t i = 0; i < map.m_buckets.Size(); ++i) {
				for (const auto& node : map.m_buckets[i]) {
					os << node.m_pair.first << ": " << node.m_pair.second << ", ";
				}
				if (i < map.m_buckets.Size() - 1) {
					os << ", ";
				}
				if (i == 10) {
					os << "...";
					break;
				}
			}
			os << " }";
			return os;
		}

		size_t Size() const { return m_size; }
		bool   Empty() const { return m_size == 0; }

		void Clear() {
			for (auto& bucket : m_buckets) {
				bucket.Clear();
			}
			m_size = 0;
		}

		Iterator Begin() {
			for (size_t i = 0; i < m_buckets.Size(); ++i) {
				if (!m_buckets[i].Empty()) {
					return Iterator(&m_buckets, i);
				}
			}
			return End();
		}

		Iterator End() { return Iterator(&m_buckets, m_buckets.Size()); }


		ConstIterator Begin() const { return const_cast<UnorderedMap*>(this)->Begin(); }
		ConstIterator End() const { return const_cast<UnorderedMap*>(this)->End(); }

		void Swap(UnorderedMap& other) {
			m_buckets.Swap(other.m_buckets);
			Spark::Swap(m_size, other.m_size);
			Spark::Swap(m_hasher, other.m_hasher);
		}

		f32  LoadFactor() const { return static_cast<f32>(m_size) / m_buckets.Size(); }
		size_t BucketCount() const { return m_buckets.Size(); }
		size_t MaxBucketCount() const { return m_buckets.MaxSize(); }
		size_t BucketSize(size_t n) const { return m_buckets[n].Size(); }
		size_t GetBucket(const Key& key) const { return GetBucketIndex(key); }

		void Reserve(size_t count) {
			size_t bucketCount = static_cast<size_t>(Math::Ceil(count / MAX_LOAD_FACTOR));
			if (bucketCount > m_buckets.Size()) {
				Rehash(bucketCount);
			}
		}

		void Rehash(size_t new_bucket_count) {
			Vector<Bucket> new_buckets(new_bucket_count);
			for (const auto& bucket : m_buckets) {
				for (const auto& node : bucket) {
					size_t new_index = m_hasher(node.m_pair.first) % new_bucket_count;
					new_buckets[new_index].PushBack(node);
				}
			}
			m_buckets = Move(new_buckets);
		}

		bool operator==(const UnorderedMap& other) const {
			if (m_size != other.m_size)
				return false;
			for (const auto& pair : *this) {
				auto it = other.Find(pair.first);
				if (it == other.End() || it->second != pair.second)
					return false;
			}
			return true;
		}

		bool operator!=(const UnorderedMap& other) const { return !(*this == other); }

		Iterator Find(const Key& key) {
			size_t index = GetBucketIndex(key);
			auto   it    = m_buckets[index].Begin();
			for (; it != m_buckets[index].End(); ++it) {
				if (it->m_pair.first == key) {
					return Iterator(&m_buckets, index, it);
				}
			}
			return End();
		}

		ConstIterator Find(const Key& key) const { return const_cast<UnorderedMap*>(this)->Find(key); }

		size_t Count(const Key& key) const { return Contains(key) ? 1 : 0; }

		Pair<Iterator, bool> InsertOrAssign(const Key& key, const Value& value) {
			auto it = Find(key);
			if (it != End()) {
				it->second = value;
				return {it, false};
			}
			Insert(key, value);
			return {Find(key), true};
		}

		template<typename... Args> Pair<Iterator, bool> Emplace(Args&&... args) {
			auto node = Node(Forward<Args>(args)...);
			auto it   = Find(node.m_pair.first);
			if (it != End()) {
				return {it, false};
			}
			Insert(Move(node.m_pair.first), Move(node.m_pair.second));
			return {Find(node.m_pair.first), true};
		}

		Iterator Erase(Iterator it) {
			if (it == End()) {
				return End();
			}
			size_t index = it.m_bucket_index;
			auto   next  = it;
			++next;
			m_buckets[index].Erase(it.m_it);
			--m_size;
			return next;
		}

		Iterator Erase(const Key& key) { return Erase(Find(key)); }

		Iterator Erase(Iterator first, Iterator last) {
			while (first != last) {
				first = Erase(first);
			}
			return last;
		}

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

	  private:
		size_t GetBucketIndex(const Key& key) const { return m_hasher(key) % m_buckets.Size(); }

	  private:
		static constexpr size_t DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32  MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		size_t         m_size;
		Hash           m_hasher;
	};

	template<typename Key, typename Value, typename Hash = std::hash<Key>> void Swap(UnorderedMap<Key, Value, Hash>& lhs, UnorderedMap<Key, Value, Hash>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_UNORDERED_MAP_HPP