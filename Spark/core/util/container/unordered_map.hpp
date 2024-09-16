#ifndef SPARK_UNORDERED_MAP_HPP
#define SPARK_UNORDERED_MAP_HPP

#include <core/util/types.hpp>
#include <initializer_list>
#include <core/util/classic/type.hpp>
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
			Node(const Key& key, Value&& value)
				: m_pair(key, Move(value)) {}
		};

		using Bucket         = List<Node>;
		using ValueType      = Pair<const Key, Value>;
		using Pointer        = ValueType*;
		using Reference      = ValueType&;
		using ConstReference = const ValueType&;

		class Iterator {
		  public:
			Iterator(Vector<Bucket>* buckets, size_t bucket_index, typename Bucket::Iterator it)
				: m_buckets(buckets)
				, m_bucket_index(bucket_index)
				, m_it(it) {
				if (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
					operator++();
				}
			}

			Reference operator*() const { return (*m_it).m_pair; }
			Pointer   operator->() const { return &((*m_it).m_pair); }

			Iterator& operator++() {
				if (m_bucket_index >= m_buckets->Size())
					return *this;

				++m_it;
				while (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
					++m_bucket_index;
					if (m_bucket_index < m_buckets->Size()) {
						m_it = (*m_buckets)[m_bucket_index].Begin();
					}
				}
				return *this;
			}

			Iterator operator++(int) {
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
			friend class UnorderedMap;
		};

		using ConstIterator = Iterator;

		UnorderedMap()
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
		}
		explicit UnorderedMap(size_t bucket_count)
			: m_buckets(bucket_count)
			, m_size(0) {
		}

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

		UnorderedMap(std::initializer_list<Pair<Key, Value>> list)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
			for (const auto& pair : list) {
				Insert(pair.first, pair.second);
			}
		}

		UnorderedMap(const std::unordered_map<Key, Value>& other)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
			for (const auto& pair : other) {
				Insert(pair.first, pair.second);
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
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			size_t index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (node.m_pair.first == key) {
					node.m_pair.second = value;
					return;
				}
			}

			m_buckets[index].PushBack(Node(key, value));
			++m_size;
		}

		bool Remove(const Key& key) {
			size_t index  = GetBucketIndex(key);
			auto&  bucket = m_buckets[index];
			for (auto it = bucket.Begin(); it != bucket.End(); ++it) {
				if ((*it).m_pair.first == key) {
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
			LOG_FATAL("Key not found");
		}

		const Value& At(const Key& key) const { return const_cast<UnorderedMap*>(this)->At(key); }

		Value& operator[](const Key& key) {
			size_t index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				LOG_INFO("Size: " << m_buckets[index].Size());
				if (node.m_pair.first == key) {
					return node.m_pair.second;
				}
			}
			m_buckets[index].PushBack(Node(key, Value()));
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
					return Iterator(&m_buckets, i, m_buckets[i].Begin());
				}
			}
			return End();
		}

		Iterator End() { return Iterator(&m_buckets, m_buckets.Size(), typename Bucket::Iterator(nullptr)); }

		ConstIterator Begin() const { return const_cast<UnorderedMap*>(this)->Begin(); }
		ConstIterator End() const { return const_cast<UnorderedMap*>(this)->End(); }

		void Swap(UnorderedMap& other) {
			m_buckets.Swap(other.m_buckets);
			Spark::Swap(m_size, other.m_size);
			Spark::Swap(m_hasher, other.m_hasher);
		}

		f32  LoadFactor() const { return static_cast<f32>(m_size) / m_buckets.Size(); }
		size_t BucketCount() const { return m_buckets.Size(); }
		size_t BucketSize(size_t n) const { return m_buckets[n].Size(); }

		void Reserve(size_t count) {
			size_t bucketCount = static_cast<size_t>(std::ceil(count / MAX_LOAD_FACTOR));
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

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

	  private:
		size_t GetBucketIndex(const Key& key) const { return m_hasher(key) % m_buckets.Size(); }

		static constexpr size_t DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32  MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		size_t         m_size;
		Hash           m_hasher;
	};

	template<typename Key, typename Value, typename Hash = std::hash<Key>> void Swap(UnorderedMap<Key, Value, Hash>& lhs, UnorderedMap<Key, Value, Hash>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_UNORDERED_MAP_HPP