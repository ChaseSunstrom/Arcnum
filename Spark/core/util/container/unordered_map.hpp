#ifndef SPARK_UNORDERED_MAP_HPP
#define SPARK_UNORDERED_MAP_HPP

#include <core/math/math.hpp>
#include <core/util/classic/compare.hpp>
#include <core/util/classic/type.hpp>
#include <core/util/memory/allocator.hpp>
#include <core/util/types.hpp>
#include <initializer_list>
#include <core/util/general.hpp>
#include "iterator.hpp"
#include "list.hpp"
#include "pair.hpp"
#include "vector.hpp"

namespace Spark {

	template<typename Key, typename Value> class UnorderedMapIterator;

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
		using AllocatorType   = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using ValueType       = Pair<const Key, Value>;
		using Pointer         = ValueType*;
		using Reference       = ValueType&;
		using ConstReference  = const ValueType&;
		using SizeType        = AllocatorTraits::SizeType;
		using DifferenceType  = AllocatorTraits::DifferenceType;
		using Bucket = List<ValueType, Allocator>;

		using Iterator        = UnorderedMapIterator<Key, Value>;
		using ConstIterator   = const Iterator;

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
				m_buckets   = other.m_buckets;
				m_size      = other.m_size;
				m_hasher    = other.m_hasher;
				m_key_equal = other.m_key_equal;
				m_allocator = other.m_allocator;
			}
			return *this;
		}

		UnorderedMap& operator=(UnorderedMap&& other) noexcept {
			if (this != &other) {
				Clear();
				m_buckets    = Move(other.m_buckets);
				m_size       = other.m_size;
				m_hasher     = Move(other.m_hasher);
				m_key_equal  = Move(other.m_key_equal);
				m_allocator  = Move(other.m_allocator);
				other.m_size = 0;
			}
			return *this;
		}

		void Insert(const Key& key, const Value& value) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			SizeType index = GetBucketIndex(key);
			for (auto& pair : m_buckets[index]) {
				if (m_key_equal(pair.first, key)) {
					pair.second = value;
					return;
				}
			}

			m_buckets[index].PushBack(ValueType(key, value));
			++m_size;
		}

		void Insert(const Key& key, Value&& value) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			SizeType index = GetBucketIndex(key);
			for (auto& pair : m_buckets[index]) {
				if (m_key_equal(pair.first, key)) {
					pair.second = Move(value);
					return;
				}
			}

			m_buckets[index].PushBack(ValueType(key, Move(value)));
			++m_size;
		}

		template<typename... Args> void Emplace(const Key& key, Args&&... args) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			SizeType index = GetBucketIndex(key);
			for (auto& pair : m_buckets[index]) {
				if (m_key_equal(pair.first, key)) {
					pair.second = Value(Forward<Args>(args)...);
					return;
				}
			}

			m_buckets[index].PushBack(ValueType(key, Value(Forward<Args>(args)...)));
			++m_size;
		}

		bool Remove(const Key& key) {
			SizeType index  = GetBucketIndex(key);
			auto&    bucket = m_buckets[index];
			for (auto it = bucket.Begin(); it != bucket.End(); ++it) {
				if (m_key_equal((*it).first, key)) {
					bucket.Erase(it);
					--m_size;
					return true;
				}
			}
			return false;
		}

		bool Erase(const Key& key) { return Remove(key); }

		bool Erase(Iterator it) {
			if (it == End())
				return false;

			auto& bucket = m_buckets[it.m_bucket_index];
			bucket.Erase(it.m_it);
			--m_size;
			return true;
		}

		Value& At(const Key& key) {
			SizeType index = GetBucketIndex(key);
			for (auto& pair : m_buckets[index]) {
				if (m_key_equal(pair.first, key)) {
					return pair.second;
				}
			}
			LOG_FATAL("Key not found");
		}

		const Value& At(const Key& key) const { return const_cast<UnorderedMap*>(this)->At(key); }

		Value& operator[](const Key& key) {
			SizeType index = GetBucketIndex(key);
			for (auto& node : m_buckets[index]) {
				if (m_key_equal(node.first, key)) {
					return node.second;
				}
			}
			m_buckets[index].PushBack(PairOf<const Key, Value>(key, Value()));
			++m_size;
			return m_buckets[index].Back().second;
		}

		bool Contains(const Key& key) const {
			SizeType index = GetBucketIndex(key);
			for (const auto& node : m_buckets[index]) {
				if (m_key_equal(node.first, key)) {
					return true;
				}
			}
			return false;
		}

		SizeType Size() const { return m_size; }
		bool     Empty() const { return m_size == 0; }

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

		f32      LoadFactor() const { return static_cast<f32>(m_size) / m_buckets.Size(); }
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
			for (auto& bucket : m_buckets) {
				for (auto it = bucket.Begin(); it != bucket.End();) {
					SizeType new_index = m_hasher(it->first) % new_bucket_count;
					new_buckets[new_index].PushBack(Move(*it));
					it = bucket.Erase(it);
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
			auto     it    = m_buckets[index].Begin();
			for (; it != m_buckets[index].End(); ++it) {
				if (m_key_equal(it->first, key)) {
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
		Hash      GetHasher() const { return m_hasher; }
		KeyEqual  GetKeyEqual() const { return m_key_equal; }

	  private:
		SizeType GetBucketIndex(const Key& key) const { return m_hasher(key) % m_buckets.Size(); }

		static constexpr SizeType DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32      MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		SizeType       m_size;
		Hash           m_hasher;
		KeyEqual       m_key_equal;
		Allocator      m_allocator;

		friend class UnorderedMapIterator<Key, Value>;
	};

	template<typename Key, typename Value> class UnorderedMapIterator : public _SPARK Iterator<ForwardIteratorTag, Pair<const Key, Value>, ptrdiff_t, Pair<const Key, Value>*, Pair<const Key, Value>&> {
	  public:
		using IteratorCategory = ForwardIteratorTag;
		using ValueType        = Pair<const Key, Value>;
		using DifferenceType   = ptrdiff_t;
		using Pointer          = ValueType*;
		using Reference        = ValueType&;
		using ConstPointer     = const ValueType*;
		using ConstReference   = const ValueType&;
		using Bucket           = UnorderedMap<Key, Value>::Bucket;

		// Constructor
		UnorderedMapIterator(Vector<Bucket>* buckets, size_t bucket_index, typename Bucket::Iterator it)
			: m_buckets(buckets)
			, m_bucket_index(bucket_index)
			, m_it(it) {
			if (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
				operator++();
			}
		}

		// Non-const operator*
		Reference operator*() { return *m_it; }

		// Const operator*
		ConstReference operator*() const { return *m_it; }

		// Non-const operator->
		Pointer operator->() { return &(*m_it); }

		// Const operator->
		ConstPointer operator->() const { return &(*m_it); }


		// Pre-increment
		UnorderedMapIterator& operator++() {
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

		// Post-increment
		UnorderedMapIterator operator++(i32) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		// Equality comparison
		bool operator==(const UnorderedMapIterator& other) const {
			return m_buckets == other.m_buckets && m_bucket_index == other.m_bucket_index && (m_bucket_index == m_buckets->Size() || m_it == other.m_it);
		}

		// Inequality comparison
		bool operator!=(const UnorderedMapIterator& other) const { return !(*this == other); }

	  private:
		Vector<Bucket>*           m_buckets;
		size_t                    m_bucket_index;
		typename Bucket::Iterator m_it;

		friend class UnorderedMap<Key, Value>;
	};

	template<typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = Equal<Key>, typename Allocator = _SPARK Allocator<Pair<const Key, Value>>>
	void Swap(UnorderedMap<Key, Value, Hash, KeyEqual, Allocator>& lhs, UnorderedMap<Key, Value, Hash, KeyEqual, Allocator>& rhs) {
		lhs.Swap(rhs);
	}
} // namespace Spark

#endif // SPARK_UNORDERED_MAP_HPP