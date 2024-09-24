#ifndef SPARK_UNORDERED_MAP_HPP
#define SPARK_UNORDERED_MAP_HPP

#include <core/math/math.hpp>
#include <core/util/classic/type.hpp>
#include <core/util/types.hpp>
#include <core/util/memory/allocator.hpp>
#include <core/util/classic/compare.hpp>
#include <initializer_list>
#include "list.hpp"
#include "pair.hpp"
#include "vector.hpp"


namespace Spark {
/**
 * @brief A hash table implementation of an unordered map.
 * 
 * @tparam Key The type of the keys in the map.
 * @tparam Value The type of the mapped values.
 * @tparam Hash The hash function type, defaults to std::hash<Key>.
 * @tparam KeyEqual The key comparison function type, defaults to Equal<Key>.
 * @tparam Allocator The allocator type, defaults to Spark Allocator<Pair<const Key, Value>>.
 */
	template<typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = Equal<Key>, typename Allocator = _SPARK Allocator<Pair<const Key, Value>>> class UnorderedMap {
	  public:
	  
		struct Node {
			Pair<const Key, Value> m_pair;
			Node(const Key& key, const Value& value)
				: m_pair(key, value) {}
			Node(const Key& key, Value&& value)
				: m_pair(key, Move(value)) {}
			Node(Key&& key, Value&& value)
				: m_pair(Move(key), Move(value)) {}
		};

		using AllocatorType  = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using Bucket         = List<Node, Allocator>;
		using ValueType      = Pair<const Key, Value>;
		using Pointer        = ValueType*;
		using Reference      = ValueType&;
		using ConstReference = const ValueType&;
		using SizeType       = AllocatorTraits::SizeType;
		using DifferenceType = AllocatorTraits::DifferenceType;

		class Iterator {
		  public:
			using IteratorCategory = std::forward_iterator_tag;
			using ValueType          = typename AllocatorTraits::ValueType;
			using PointerType        = typename AllocatorTraits::PointerType;
			using ReferenceType      = typename AllocatorTraits::ReferenceType;
			using ConstReferenceType = typename AllocatorTraits::ConstReferenceType;
			using SizeType           = typename AllocatorTraits::SizeType;
			using DifferenceType     = typename AllocatorTraits::DifferenceType;

			Iterator(Vector<Bucket>* buckets, SizeType bucket_index, typename Bucket::Iterator it)
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
			SizeType                    m_bucket_index;
			typename Bucket::Iterator m_it;
			friend class UnorderedMap;
		};

		using ConstIterator = Iterator;

		explicit UnorderedMap(SizeType bucket_count = DEFAULT_BUCKET_COUNT, const Hash& hasher = Hash(), const KeyEqual& key_equal = KeyEqual(), const Allocator& allocator = Allocator())
			: m_buckets(bucket_count)
			, m_size(0)
			, m_hasher(hasher)
			, m_key_equal(key_equal)
			, m_allocator(allocator) {}

		UnorderedMap(const UnorderedMap& other)
			: m_buckets(other.m_buckets)
			, m_size(other.m_size)
			, m_hasher(other.m_hasher)
			, m_key_equal(other.m_key_equal)
			, m_allocator(other.m_allocator) {}

		UnorderedMap(UnorderedMap&& other) noexcept
			: m_buckets(Move(other.m_buckets))
			, m_size(other.m_size)
			, m_hasher(Move(other.m_hasher))
			, m_key_equal(Move(other.m_key_equal))
			, m_allocator(Move(other.m_allocator)) {
			other.m_size = 0;
		}

		UnorderedMap(std::initializer_list<Pair<Key, Value>> list)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
			for (const auto& pair : list) {
				Insert(pair.first, pair.second);
			}
		}

		UnorderedMap(const std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>& other)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0)
			, m_hasher(other.hash_function())
			, m_key_equal(other.key_eq())
			, m_allocator(other.get_allocator()) {
			for (const auto& pair : other) {
				Insert(pair.first, pair.second); 
			}
		}

		~UnorderedMap() { Clear(); }

		UnorderedMap& operator=(const UnorderedMap& other) {
			if (this != &other) {
				Clear();
				m_buckets = other.m_buckets;
				m_size = other.m_size;
				m_hasher = other.m_hasher;
				m_key_equal = other.m_key_equal;
				m_allocator = other.m_allocator;
			}
			return *this;
		}

		UnorderedMap& operator=(UnorderedMap&& other) noexcept {
			if (this != &other) {
				Clear();
				m_buckets = Move(other.m_buckets);
				m_size = other.m_size;
				m_hasher = Move(other.m_hasher);
				m_key_equal = Move(other.m_key_equal);
				m_allocator = Move(other.m_allocator);
				other.m_size = 0;
			}
			return *this;
		}

		void Insert(const Key& key, const Value& value) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			SizeType index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (m_key_equal(node.m_pair.first, key)) {
					node.m_pair.second = value;
					return;
				}
			}

			m_buckets[index].PushBack(Node(key, value));
			++m_size;
		}

		bool Remove(const Key& key) {
			SizeType index  = GetBucketIndex(key);
			auto&  bucket = m_buckets[index];
			for (auto it = bucket.Begin(); it != bucket.End(); ++it) {
				if (m_key_equal((*it).m_pair.first, key)) {
					bucket.Erase(it);
					--m_size;
					return true;
				}
			}
			return false;
		}

		Value& At(const Key& key) {
			SizeType index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (m_key_equal(node.m_pair.first, key)) {
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
				if (m_key_equal(node.m_pair.first, key)) {
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
				if (m_key_equal(node.m_pair.first, key)) {
					return true;
				}
			}
			return false;
		}

		SizeType Size() const { return m_size; }
		bool   Empty() const { return m_size == 0; }

		void Clear() {
			for (auto& bucket : m_buckets) {
				bucket.Clear();
			}
			m_size = 0;
		}

		Iterator Begin() {
			for (SizeType i = 0; i < m_buckets.Size(); ++i) {
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
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_hasher, other.m_hasher);
			_SPARK Swap(m_key_equal, other.m_key_equal);
			_SPARK Swap(m_allocator, other.m_allocator);
		}

		f32    LoadFactor() const { return static_cast<f32>(m_size) / m_buckets.Size(); }
		SizeType BucketCount() const { return m_buckets.Size(); }
		SizeType BucketSize(SizeType n) const { return m_buckets[n].Size(); }

		void Reserve(SizeType count) {
			SizeType bucketCount = static_cast<SizeType>(_MATH Ceil(count / MAX_LOAD_FACTOR));
			if (bucketCount > m_buckets.Size()) {
				Rehash(bucketCount);
			}
		}

		void Rehash(SizeType new_bucket_count) {
			Vector<Bucket> new_buckets(new_bucket_count);
			for (const auto& bucket : m_buckets) {
				for (const auto& node : bucket) {
					SizeType new_index = m_hasher(node.m_pair.first) % new_bucket_count;
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
				if (it == other.End() || !m_key_equal(it->second, pair.second))
					return false;
			}
			return true;
		}

		bool operator!=(const UnorderedMap& other) const { return !(*this == other); }

		Iterator Find(const Key& key) {
			SizeType index = GetBucketIndex(key);
			auto   it    = m_buckets[index].Begin();
			for (; it != m_buckets[index].End(); ++it) {
				if (m_key_equal(it->m_pair.first, key)) {
					return Iterator(&m_buckets, index, it);
				}
			}
			return End();
		}

		ConstIterator Find(const Key& key) const { return const_cast<UnorderedMap*>(this)->Find(key); }

		SizeType Count(const Key& key) const { return Contains(key) ? 1 : 0; }

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

		Allocator GetAllocator() const { return m_allocator; }
		Hash GetHasher() const { return m_hasher; }
		KeyEqual GetKeyEqual() const { return m_key_equal; }

	  private:
		SizeType GetBucketIndex(const Key& key) const { return m_hasher(key) % m_buckets.Size(); }

		static constexpr SizeType DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32    MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		SizeType         m_size;
		Hash           m_hasher;
		KeyEqual       m_key_equal;
		Allocator      m_allocator;
	};

	template<typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = Equal<Key>, typename Allocator = _SPARK Allocator<Pair<const Key, Value>>> void Swap(UnorderedMap<Key, Value, Hash, KeyEqual, Allocator>& lhs, UnorderedMap<Key, Value, Hash, KeyEqual, Allocator>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_UNORDERED_MAP_HPP