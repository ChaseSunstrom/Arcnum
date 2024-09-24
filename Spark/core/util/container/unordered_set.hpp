#ifndef SPARK_UNORDERED_SET_HPP
#define SPARK_UNORDERED_SET_HPP

#include <core/util/types.hpp>
#include <core/util/classic/compare.hpp>
#include <unordered_set>
#include <xhash>
#include "list.hpp"
#include "vector.hpp"


namespace Spark {
	/**
	 * @brief An unordered set implementation using hash table.
	 * 
	 * @tparam _Ty The type of elements in the set.
	 * @tparam Hash The hash function type, defaults to std::hash<_Ty>.
	 * @tparam KeyEqual The key equality comparison function type, defaults to Equal<_Ty>.
	 * @tparam Allocator The allocator type, defaults to _SPARK Allocator<_Ty>.
	 */
	template<typename _Ty, typename Hash = std::hash<_Ty>, typename KeyEqual = Equal<_Ty>, typename Allocator = _SPARK Allocator<_Ty>> class UnorderedSet {
	  public:
		/**
		 * @brief Node structure for the unordered set.
		 */
		struct Node {
			_Ty value;
			Node(const _Ty& value)
				: value(value) {}
			Node(_Ty&& value)
				: value(Move(value)) {}
		};
		
		using AllocatorType  = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using Bucket         = List<Node, Allocator>;
		using ValueType      = typename AllocatorTraits::ValueType;
		using Pointer        = typename AllocatorTraits::Pointer;
		using ConstPointer   = typename AllocatorTraits::ConstPointer;
		using Reference      = typename AllocatorTraits::Reference;
		using ConstReference = typename AllocatorTraits::ConstReference;
		using SizeType       = typename AllocatorTraits::SizeType;
		using DifferenceType = typename AllocatorTraits::DifferenceType;
		
		
		class Iterator {
		  public:
			using IteratorCategory = std::forward_iterator_tag;
			using ValueType          = typename AllocatorTraits::ValueType;
			using Pointer        = typename AllocatorTraits::Pointer;
			using Reference      = typename AllocatorTraits::Reference;
			using ConstReference = typename AllocatorTraits::ConstReference;
			using SizeType           = typename AllocatorTraits::SizeType;
			using DifferenceType     = typename AllocatorTraits::DifferenceType;

			Iterator(Vector<Bucket>* buckets, size_t bucket_index, typename Bucket::Iterator it)
				: m_buckets(buckets)
				, m_bucket_index(bucket_index)
				, m_it(it) {
				if (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
					operator++();
				}
			}
			ConstReference operator*() const { return (*m_it).value; }
			const Pointer  operator->() const { return &((*m_it).value); }
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
			friend class UnorderedSet;
		};

		using ConstIterator = Iterator;

		explicit UnorderedSet(size_t bucket_count = DEFAULT_BUCKET_COUNT, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual(), const Allocator& alloc = Allocator())
			: m_buckets(bucket_count)
			, m_size(0)
			, m_hasher(hash)
			, m_key_equal(equal)
			, m_allocator(alloc) {}

		UnorderedSet(const UnorderedSet& other)
			: m_buckets(other.m_buckets)
			, m_size(other.m_size)
			, m_hasher(other.m_hasher)
			, m_key_equal(other.m_key_equal)
			, m_allocator(other.m_allocator) {}

		UnorderedSet(UnorderedSet&& other) noexcept
			: m_buckets(Move(other.m_buckets))
			, m_size(other.m_size)
			, m_hasher(Move(other.m_hasher))
			, m_key_equal(Move(other.m_key_equal))
			, m_allocator(Move(other.m_allocator)) {
			other.m_size = 0;
		}

		UnorderedSet(std::initializer_list<_Ty> init, size_t bucket_count = DEFAULT_BUCKET_COUNT,
					 const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual(), const Allocator& alloc = Allocator())
			: UnorderedSet(bucket_count, hash, equal, alloc) {
			for (const auto& value : init) {
				Insert(value);
			}
		}

		UnorderedSet& operator=(const UnorderedSet& other) {
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

		UnorderedSet& operator=(UnorderedSet&& other) noexcept {
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

		~UnorderedSet() { Clear(); }

		void Clear() {
			for (auto& bucket : m_buckets) {
				bucket.Clear();
			}
			m_size = 0;
		}

		void Insert(ConstReference value) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			size_t index = GetBucketIndex(value);
			for (const auto& item : m_buckets[index]) {
				if (m_key_equal(item.value, value)) {
					return;
				}
			}
			m_buckets[index].PushBack(value);
			++m_size;
		}

		void Insert(_Ty&& value) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			size_t index = GetBucketIndex(value);
			for (const auto& item : m_buckets[index]) {
				if (m_key_equal(item, value)) {
					return;
				}
			}
			m_buckets[index].PushBack(Move(value));
			++m_size;
		}

		bool Erase(ConstReference value) {
			size_t index = GetBucketIndex(value);
			auto& bucket = m_buckets[index];
			for (auto it = bucket.Begin(); it != bucket.End(); ++it) {
				if (m_key_equal(*it, value)) {
					bucket.Erase(it);
					--m_size;
					return true;
				}
			}
			return false;
		}

		bool Erase(Iterator it) {
			if (it.m_bucket_index >= m_buckets.Size()) {
				return false;
			}
			auto& bucket = m_buckets[it.m_bucket_index];
			if (it.m_it == bucket.End()) {
				return false;
			}
			bucket.Erase(it.m_it);
			--m_size;
			return true;
		}

		bool Contains(ConstReference value) const {
			size_t index = GetBucketIndex(value);
			for (const auto& item : m_buckets[index]) {
				if (m_key_equal(item, value)) {
					return true;
				}
			}
			return false;
		}

		void Reserve(size_t count) {
			size_t bucketCount = static_cast<size_t>(_MATH Ceil(count / MAX_LOAD_FACTOR));
			if (bucketCount > m_buckets.Size()) {
				Rehash(bucketCount);
			}
		}

		void Rehash(size_t new_bucket_count) {
			Vector<Bucket> new_buckets(new_bucket_count);
			for (const auto& bucket : m_buckets) {
				for (const auto& value : bucket) {
					size_t new_index = m_hasher(value) % new_bucket_count;
					new_buckets[new_index].PushBack(value);
				}
			}
			m_buckets = Move(new_buckets);
		}

		UnorderedSet& operator+=(const UnorderedSet& other) {
			for (const auto& value : other) {
				Insert(value);
			}
			return *this;
		}

		UnorderedSet operator+(const UnorderedSet& other) const {
			UnorderedSet result = *this;
			result += other;
			return result;
		}

		UnorderedSet& operator-=(const UnorderedSet& other) {
			for (const auto& value : other) {
				Erase(value);
			}
			return *this;
		}

		UnorderedSet operator-(const UnorderedSet& other) const {
			UnorderedSet result = *this;
			result -= other;
			return result;
		}

		UnorderedSet operator+(ConstReference value) const {
			UnorderedSet result = *this;
			result.Insert(value);
			return result;
		}

		UnorderedSet operator+(_Ty&& value) const {
			UnorderedSet result = *this;
			result.Insert(Move(value));
			return result;
		}

		UnorderedSet& operator+=(UnorderedSet&& other) {
			for (const auto& value : other) {
				Insert(value);
			}
			return *this;
		}

		UnorderedSet& operator+=(ConstReference value) {
			Insert(value);
			return *this;
		}

		UnorderedSet& operator+=(_Ty&& value) {
			Insert(Move(value));
			return *this;
		}

		UnorderedSet& operator-=(ConstReference value) {
			Erase(value);
			return *this;
		}

		Iterator Find(const _Ty& value) {
			size_t index = GetBucketIndex(value);
			auto it = m_buckets[index].Begin();
			for (; it != m_buckets[index].End(); ++it) {
				if (m_key_equal(*it, value)) {
					return Iterator(&m_buckets, index, it);
				}
			}
			return End();
		}

		ConstIterator Find(const _Ty& value) const {
			return const_cast<UnorderedSet*>(this)->Find(value);
		}

		size_t Count(const _Ty& value) const {
			return Contains(value) ? 1 : 0;
		}

		void Swap(UnorderedSet& other) {
			m_buckets.Swap(other.m_buckets);
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_hasher, other.m_hasher);
			_SPARK Swap(m_key_equal, other.m_key_equal);
			_SPARK Swap(m_allocator, other.m_allocator);
		}

		Reference operator[](SizeType index) {
			if (index >= m_size) {
				LOG_FATAL("Index out of range");
			}
			for (auto& bucket : m_buckets) {
				if (index < bucket.Size()) {
					return bucket[index].value;
				}
				index -= bucket.Size();
			}
			LOG_FATAL("Index out of range"); // This should never be reached
			static _Ty dummy; // To avoid compiler warnings
			return dummy;
		}

		ConstReference operator[](SizeType index) const {
			return const_cast<UnorderedSet*>(this)->operator[](index);
		}

		bool operator==(const UnorderedSet& other) const {
			if (m_size != other.m_size)
				return false;
			for (const auto& value : *this) {
				if (!other.Contains(value))
					return false;
			}
			return true;
		}

		bool operator!=(const UnorderedSet& other) const {
			return !(*this == other);
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
		ConstIterator Begin() const { return const_cast<UnorderedSet*>(this)->Begin(); }
		ConstIterator End() const { return const_cast<UnorderedSet*>(this)->End(); }
		
		Iterator begin() { return Begin(); }
		Iterator end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

		Allocator GetAllocator() const { return m_allocator; }

		f32 LoadFactor() const { return static_cast<f32>(m_size) / m_buckets.Size(); }
		size_t BucketCount() const { return m_buckets.Size(); }
		size_t Size() const { return m_size; }
		bool Empty() const { return m_size == 0; }

	  private:
		size_t GetBucketIndex(const _Ty& value) const {
			return m_hasher(value) % m_buckets.Size();
		}


	  private:
		static constexpr size_t DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32    MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		size_t         m_size;
		Hash           m_hasher;
		KeyEqual       m_key_equal;
		Allocator      m_allocator;
	};
} // namespace Spark

#endif