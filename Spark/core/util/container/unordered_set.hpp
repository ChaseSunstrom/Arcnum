#ifndef SPARK_UNORDERED_SET_HPP
#define SPARK_UNORDERED_SET_HPP

#include <core/util/types.hpp>
#include <unordered_set>
#include <xhash>
#include "list.hpp"
#include "vector.hpp"

namespace Spark {
	template<typename T, typename Hash = std::hash<T>> class UnorderedSet {
	  public:
		struct Node {
			T value;
			Node(const T& value)
				: value(value) {}
			Node(T&& value)
				: value(Move(value)) {}
		};
		
		using Bucket         = List<Node>;
		using ValueType      = T;
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
			Reference operator*() const { return (*m_it).value; }
			Pointer   operator->() const { return &((*m_it).value); }
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
			Iterator& operator++(i32) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const Iterator& other) const { return m_bucket_index == other.m_bucket_index && m_it == other.m_it; }
			bool operator!=(const Iterator& other) const { return m_bucket_index != other.m_bucket_index || m_it != other.m_it; }

		  private:
			Vector<Bucket>*           m_buckets;
			size_t                    m_bucket_index;
			typename Bucket::Iterator m_it;
			friend class UnorderedSet;
		};

		UnorderedSet()
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {}

		UnorderedSet(size_t bucket_count)
			: m_buckets(bucket_count)
			, m_size(0) {}

		UnorderedSet(const UnorderedSet& other)
			: m_buckets(other.m_buckets)
			, m_size(other.m_size) {}

		UnorderedSet(UnorderedSet&& other) noexcept
			: m_buckets(Move(other.m_buckets))
			, m_size(other.m_size) {
			other.m_size = 0;
		}

		UnorderedSet(const std::unordered_set<T, Hash>& other)
			: m_buckets(DEFAULT_BUCKET_COUNT)
			, m_size(0) {
			for (const auto& value : other) {
				Insert(value);
			}
		}

		UnorderedSet& operator=(const UnorderedSet& other) {
			if (this != &other) {
				m_buckets = other.m_buckets;
				m_size    = other.m_size;
			}
			return *this;
		}

		UnorderedSet& operator=(UnorderedSet&& other) noexcept {
			if (this != &other) {
				m_buckets    = Move(other.m_buckets);
				m_size       = other.m_size;
				other.m_size = 0;
			}
			return *this;
		}

		~UnorderedSet() {
			Clear(); }

	  private:
		static constexpr size_t DEFAULT_BUCKET_COUNT = 16;
		static constexpr f32    MAX_LOAD_FACTOR      = 0.75f;

		Vector<Bucket> m_buckets;
		size_t         m_size;
		Hash           m_hash;
	};
} // namespace Spark

#endif