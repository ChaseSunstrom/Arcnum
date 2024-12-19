#ifndef SPARK_UNORDERED_SET_HPP
#define SPARK_UNORDERED_SET_HPP

#include <core/util/classic/compare.hpp>
#include <core/util/types.hpp>
#include <unordered_set>
#include <core/util/classic/hash.hpp>
#include "list.hpp"
#include "vector.hpp"

namespace Spark {

	template<typename _Ty> class UnorderedSetIterator;

	/**
	 * @brief An unordered set implementation using a hash table.
	 *
	 * @tparam _Ty The type of elements in the set.
	 * @tparam Hash The hash function type, defaults to std::hash<_Ty>.
	 * @tparam Allocator The allocator type, defaults to _SPARK Allocator<_Ty>.
	 */
	template<typename _Ty, typename Hash = _SPARK Hash<_Ty>, typename Allocator = _SPARK Allocator<_Ty>> class UnorderedSet {
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

		using AllocatorType   = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using Bucket          = List<Node, Allocator>;
		using ValueType       = _Ty;
		using Pointer         = ValueType*;
		using ConstPointer    = const ValueType*;
		using Reference       = ValueType&;
		using ConstReference  = const ValueType&;
		using SizeType        = typename AllocatorTraits::SizeType;
		using DifferenceType  = typename AllocatorTraits::DifferenceType;

		using Iterator        = UnorderedSetIterator<_Ty>;
		using ConstIterator   = const Iterator;

		explicit UnorderedSet(size_t bucket_count = DEFAULT_BUCKET_COUNT, const Hash& hash = Hash(), const Allocator& alloc = Allocator())
			: m_buckets(bucket_count)
			, m_size(0)
			, m_hasher(hash)
			, m_allocator(alloc) {}

		UnorderedSet(const UnorderedSet& other)
			: m_buckets(other.m_buckets)
			, m_size(other.m_size)
			, m_hasher(other.m_hasher)
			, m_allocator(other.m_allocator) {}

		UnorderedSet(UnorderedSet&& other) noexcept
			: m_buckets(Move(other.m_buckets))
			, m_size(other.m_size)
			, m_hasher(Move(other.m_hasher))
			, m_allocator(Move(other.m_allocator)) {
			other.m_size = 0;
		}

		UnorderedSet(std::initializer_list<_Ty> init, size_t bucket_count = DEFAULT_BUCKET_COUNT, const Hash& hash = Hash(), const Allocator& alloc = Allocator())
			: UnorderedSet(bucket_count, hash, alloc) {
			for (const auto& value : init) {
				Insert(value);
			}
		}

		UnorderedSet& operator=(const UnorderedSet& other) {
			if (this != &other) {
				Clear();
				m_buckets   = other.m_buckets;
				m_size      = other.m_size;
				m_hasher    = other.m_hasher;
				m_allocator = other.m_allocator;
			}
			return *this;
		}

		UnorderedSet& operator=(UnorderedSet&& other) noexcept {
			if (this != &other) {
				Clear();
				m_buckets    = Move(other.m_buckets);
				m_size       = other.m_size;
				m_hasher     = Move(other.m_hasher);
				m_allocator  = Move(other.m_allocator);
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
				if (item.value == value) {
					return;
				}
			}
			m_buckets[index].PushBack(Node(value));
			++m_size;
		}

		void Insert(_Ty&& value) {
			if (LoadFactor() > MAX_LOAD_FACTOR) {
				Rehash(m_buckets.Size() * 2);
			}

			size_t index = GetBucketIndex(value);
			for (const auto& item : m_buckets[index]) {
				if (item.value == value) {
					return;
				}
			}
			m_buckets[index].PushBack(Node(Move(value)));
			++m_size;
		}

		bool Erase(ConstReference value) {
			size_t index  = GetBucketIndex(value);
			auto&  bucket = m_buckets[index];
			for (auto it = bucket.Begin(); it != bucket.End(); ++it) {
				if ((*it).value == value) {
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
				if (item.value == value) {
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
				for (const auto& node : bucket) {
					size_t new_index = m_hasher(node.value) % new_bucket_count;
					new_buckets[new_index].PushBack(node);
				}
			}
			m_buckets = Move(new_buckets);
		}

		Iterator Find(const _Ty& value) {
			size_t index = GetBucketIndex(value);
			auto   it    = m_buckets[index].Begin();
			for (; it != m_buckets[index].End(); ++it) {
				if ((*it).value == value) {
					return Iterator(&m_buckets, index, it);
				}
			}
			return End();
		}

		ConstIterator Find(const _Ty& value) const { return const_cast<UnorderedSet*>(this)->Find(value); }

		size_t Count(const _Ty& value) const { return Contains(value) ? 1 : 0; }

		void Swap(UnorderedSet& other) {
			m_buckets.Swap(other.m_buckets);
			_SPARK Swap(m_size, other.m_size);
			_SPARK Swap(m_hasher, other.m_hasher);
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
			static _Ty dummy;                // To avoid compiler warnings
			return dummy;
		}

		ConstReference operator[](SizeType index) const { return const_cast<UnorderedSet*>(this)->operator[](index); }

		bool operator==(const UnorderedSet& other) const {
			if (m_size != other.m_size)
				return false;
			for (const auto& value : *this) {
				if (!other.Contains(value))
					return false;
			}
			return true;
		}

		bool operator!=(const UnorderedSet& other) const { return !(*this == other); }

		Iterator Begin() {
			for (size_t i = 0; i < m_buckets.Size(); ++i) {
				if (!m_buckets[i].Empty()) {
					return Iterator(&m_buckets, i, m_buckets[i].Begin());
				}
			}
			return End();
		}

		Iterator      End() { return Iterator(&m_buckets, m_buckets.Size(), typename Bucket::Iterator(nullptr)); }
		ConstIterator Begin() const { return const_cast<UnorderedSet*>(this)->Begin(); }
		ConstIterator End() const { return const_cast<UnorderedSet*>(this)->End(); }

		Iterator      begin() { return Begin(); }
		Iterator      end() { return End(); }
		ConstIterator begin() const { return Begin(); }
		ConstIterator end() const { return End(); }

		Allocator GetAllocator() const { return m_allocator; }

		f32    LoadFactor() const { return static_cast<f32>(m_size) / m_buckets.Size(); }
		size_t BucketCount() const { return m_buckets.Size(); }
		size_t Size() const { return m_size; }
		bool   Empty() const { return m_size == 0; }

	  private:
		size_t GetBucketIndex(const _Ty& value) const { return m_hasher(value) % m_buckets.Size(); }

	  private:
		static constexpr size_t DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32    MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		size_t         m_size;
		Hash           m_hasher;
		Allocator      m_allocator;

		friend class UnorderedSetIterator<_Ty>;
	};

	template<typename _Ty> class UnorderedSetIterator : public _SPARK Iterator<ForwardIteratorTag, _Ty, ptrdiff_t, _Ty*, _Ty&> {
	  public:
		using IteratorCategory = ForwardIteratorTag;
		using ValueType        = _Ty;
		using DifferenceType   = ptrdiff_t;
		using Pointer          = ValueType*;
		using Reference        = ValueType&;
		using ConstPointer     = const ValueType*;
		using ConstReference   = const ValueType&;
		using Bucket           = typename UnorderedSet<_Ty>::Bucket;

		// Constructor
		UnorderedSetIterator(Vector<Bucket>* buckets, size_t bucket_index, typename Bucket::Iterator it)
			: m_buckets(buckets)
			, m_bucket_index(bucket_index)
			, m_it(it) {
			if (m_bucket_index < m_buckets->Size() && m_it == (*m_buckets)[m_bucket_index].End()) {
				operator++();
			}
		}

		// Non-const operator*
		ConstReference operator*() const { return m_it->value; }

		// Non-const operator->
		ConstPointer operator->() const { return &(m_it->value); }

		// Pre-increment
		UnorderedSetIterator& operator++() {
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
		UnorderedSetIterator operator++(int) {
			UnorderedSetIterator tmp = *this;
			++(*this);
			return tmp;
		}

		// Equality comparison
		bool operator==(const UnorderedSetIterator& other) const {
			return m_buckets == other.m_buckets && m_bucket_index == other.m_bucket_index && (m_bucket_index == m_buckets->Size() || m_it == other.m_it);
		}

		// Inequality comparison
		bool operator!=(const UnorderedSetIterator& other) const { return !(*this == other); }

	  private:
		Vector<Bucket>*           m_buckets;
		size_t                    m_bucket_index;
		typename Bucket::Iterator m_it;

		friend class UnorderedSet<_Ty>;
	};

	template<typename _Ty, typename Hash = _SPARK Hash<_Ty>, typename Allocator = _SPARK Allocator<_Ty>> void Swap(UnorderedSet<_Ty, Hash, Allocator>& lhs, UnorderedSet<_Ty, Hash, Allocator>& rhs) {
		lhs.Swap(rhs);
	}

} // namespace Spark

#endif // SPARK_UNORDERED_SET_HPP
