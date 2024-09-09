#ifndef SPARK_MAP_HPP
#define SPARK_MAP_HPP

#include <map>
#include <initializer_list>
#include <core/util/types.hpp>
#include <core/util/classic/compare.hpp>
#include "pair.hpp"

namespace Spark {
	template<typename Key, typename Value, typename Compare = Less<Key>> class Map {
	private:
		enum class Color {
			RED, BLACK
		};

		struct Node {
			Pair<const Key, Value> m_pair;
			Node*                  m_left;
			Node*                  m_right;
			Node*                  m_parent;
			Color                  m_color;

			Node(const Key& key, const Value& value)
				: m_pair(key, value)
				, m_left(nullptr)
				, m_right(nullptr)
				, m_parent(nullptr)
				, m_color(Color::RED) {}
		};

		using Pointer   = Node*;
		using Reference = Node&;

	public:
		class Iterator {
		public:
			Iterator(Pointer ptr, const Map* map)
				: m_ptr(ptr)
				, m_map(map) {}

			const Pair<const Key, Value>& operator*() const { return m_ptr->m_pair; }
			const Pair<const Key, Value>* operator->() const { return &(m_ptr->m_pair); }

			Iterator& operator++() {
				m_ptr = m_map->NextNode(m_ptr);
				return *this;
			}

			Iterator operator++(i32) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			Iterator& operator--() {
				if (m_ptr == nullptr) {
					m_ptr = m_map->m_root;
					while (m_ptr && m_ptr->m_right) { m_ptr = m_ptr->m_right; }
				} else { m_ptr = m_map->PrevNode(m_ptr); }
				return *this;
			}

			Iterator operator--(i32) {
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}

			bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
			bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }

		private:
			Pointer    m_ptr;
			const Map* m_map;
		};

		using ConstIterator = const Iterator;

		Map()
			: m_root(nullptr)
			, m_size(0) {}

		Map(const Map& other)
			: m_root(nullptr)
			, m_size(0) { for (const auto& pair : other) { Insert(pair.first, pair.second); } }

		Map(const std::initializer_list<Pair<const Key, Value>>& list)
			: m_root(nullptr)
			, m_size(0) { for (const auto& pair : list) { Insert(pair.first, pair.second); } }

		Map(const std::map<Key, Value>& map)
			: m_root(nullptr)
			, m_size(0) { for (const auto& pair : map) { Insert(pair.first, pair.second); } }

		~Map() { Clear(); }

		Map& operator=(const Map& other) {
			if (this != &other) {
				Clear();
				for (const auto& pair : other) { Insert(pair.first, pair.second); }
			}
			return *this;
		}

		Map(Map&& other) noexcept
			: m_root(other.m_root)
			, m_size(other.m_size) {
			other.m_root = nullptr;
			other.m_size = 0;
		}

		Map& operator=(Map&& other) noexcept {
			if (this != &other) {
				Clear();
				m_root       = other.m_root;
				m_size       = other.m_size;
				other.m_root = nullptr;
				other.m_size = 0;
			}
			return *this;
		}

		void Insert(const Key& key, const Value& value) {
			Pointer new_node = new Node(key, value);
			if (m_root == nullptr) {
				m_root          = new_node;
				m_root->m_color = Color::BLACK;
			} else {
				Pointer current = m_root;
				Pointer parent  = nullptr;
				while (current != nullptr) {
					parent = current;
					if (Compare{}(key, current->m_pair.first)) { current = current->m_left; } else if (Compare{}(current->m_pair.first, key)) { current = current->m_right; } else {
						delete new_node;
						return;
					}
				}
				new_node->m_parent = parent;
				if (Compare{}(key, parent->m_pair.first)) { parent->m_left = new_node; } else { parent->m_right = new_node; }
				FixInsertion(new_node);
			}
			++m_size;
		}

		bool Remove(const Key& key) {
			Pointer node = FindNode(key);
			if (node == nullptr) { return false; }
			RemoveNode(node);
			--m_size;
			return true;
		}

		Value& At(const Key& key) {
			Pointer node = FindNode(key);
			if (node == nullptr) { LOG_FATAL("Key not found"); }
			return node->m_pair.second;
		}

		const Value& At(const Key& key) const { return const_cast<Map*>(this)->At(key); }

		Value& operator[](const Key& key) {
			Pointer node = FindNode(key);
			if (node == nullptr) {
				Insert(key, Value());
				node = FindNode(key);
			}
			return node->m_pair.second;
		}

		bool Contains(const Key& key) const { return FindNode(key) != nullptr; }

		size_t Size() const { return m_size; }
		bool   Empty() const { return m_size == 0; }

		void Clear() {
			DeleteTree(m_root);
			m_root = nullptr;
			m_size = 0;
		}

		friend std::ostream& operator<<(std::ostream& os, const Map& map) {
			os << "{ ";
			for (size_t i = 0; i < map.m_size; ++i) {
				auto it = map.Begin();
				for (size_t j = 0; j < i; ++j) { ++it; }
				os << it->first << ": " << it->second;
				if (i < map.m_size - 1) { os << ", "; }
				if (i == 10) {
					os << "...";
					break;
				}
			}
			os << " }";
			return os;
		}

		Iterator Begin() {
			Pointer leftmost = m_root;
			while (leftmost && leftmost->m_left) { leftmost = leftmost->m_left; }
			return Iterator(leftmost, this);
		}

		Iterator End() { return Iterator(nullptr, this); }

		ConstIterator Begin() const {
			Pointer leftmost = m_root;
			while (leftmost && leftmost->m_left) { leftmost = leftmost->m_left; }
			return ConstIterator(leftmost, this);
		}

		ConstIterator End() const { return ConstIterator(nullptr, this); }


		Iterator begin() const { return Begin(); }
		Iterator end() const { return End(); }

		void Swap(Map& other) {
			Spark::Swap(m_root, other.m_root);
			Spark::Swap(m_size, other.m_size);
		}

		Iterator Find(const Key& key) const { return Iterator(FindNode(key), this); }

		size_t Count(const Key& key) const { return Contains(key) ? 1 : 0; }

		Pair<Iterator, bool> InsertOrAssign(const Key& key, const Value& value) {
			Pointer node = FindNode(key);
			if (node != nullptr) {
				node->m_pair.second = value;
				return {Iterator(node, this), false};
			}
			Insert(key, value);
			return {Find(key), true};
		}

		template<typename... Args> Pair<Iterator, bool> Emplace(Args&&... args) {
			auto    pair = Pair<const Key, Value>(Forward<Args>(args)...);
			Pointer node = FindNode(pair.first);
			if (node != nullptr) { return {Iterator(node, this), false}; }
			Insert(pair.first, pair.second);
			return {Find(pair.first), true};
		}

		bool operator==(const Map& other) const {
			if (m_size != other.m_size) { return false; }
			for (const auto& pair : *this) {
				auto it = other.Find(pair.first);
				if (it == other.End() || it->second != pair.second) { return false; }
			}
			return true;
		}

		bool operator!=(const Map& other) const { return !(*this == other); }

	private:
		Pointer FindNode(const Key& key) const {
			Pointer current = m_root;
			while (current != nullptr) {
				if (Compare{}(key, current->m_pair.first)) { current = current->m_left; } else
					if (Compare{}(current->m_pair.first, key)) { current = current->m_right; } else { return current; }
			}
			return nullptr;
		}

		void RotateLeft(Pointer node) {
			Pointer right_child = node->m_right;
			node->m_right       = right_child->m_left;
			if (right_child->m_left != nullptr) { right_child->m_left->m_parent = node; }
			right_child->m_parent = node->m_parent;
			if (node->m_parent == nullptr) { m_root = right_child; } else
				if (node == node->m_parent->m_left) { node->m_parent->m_left = right_child; } else { node->m_parent->m_right = right_child; }
			right_child->m_left = node;
			node->m_parent      = right_child;
		}

		void RotateRight(Pointer node) {
			Pointer left_child = node->m_left;
			node->m_left       = left_child->m_right;
			if (left_child->m_right != nullptr) { left_child->m_right->m_parent = node; }
			left_child->m_parent = node->m_parent;
			if (node->m_parent == nullptr) { m_root = left_child; } else
				if (node == node->m_parent->m_right) { node->m_parent->m_right = left_child; } else { node->m_parent->m_left = left_child; }
			left_child->m_right = node;
			node->m_parent      = left_child;
		}

		void FixInsertion(Pointer node) {
			while (node != m_root && node->m_parent->m_color == Color::RED) {
				if (node->m_parent == node->m_parent->m_parent->m_left) {
					Pointer uncle = node->m_parent->m_parent->m_right;
					if (uncle != nullptr && uncle->m_color == Color::RED) {
						node->m_parent->m_color           = Color::BLACK;
						uncle->m_color                    = Color::BLACK;
						node->m_parent->m_parent->m_color = Color::RED;
						node                              = node->m_parent->m_parent;
					} else {
						if (node == node->m_parent->m_right) {
							node = node->m_parent;
							RotateLeft(node);
						}
						node->m_parent->m_color           = Color::BLACK;
						node->m_parent->m_parent->m_color = Color::RED;
						RotateRight(node->m_parent->m_parent);
					}
				} else {
					Pointer uncle = node->m_parent->m_parent->m_left;
					if (uncle != nullptr && uncle->m_color == Color::RED) {
						node->m_parent->m_color           = Color::BLACK;
						uncle->m_color                    = Color::BLACK;
						node->m_parent->m_parent->m_color = Color::RED;
						node                              = node->m_parent->m_parent;
					} else {
						if (node == node->m_parent->m_left) {
							node = node->m_parent;
							RotateRight(node);
						}
						node->m_parent->m_color           = Color::BLACK;
						node->m_parent->m_parent->m_color = Color::RED;
						RotateLeft(node->m_parent->m_parent);
					}
				}
			}
			m_root->m_color = Color::BLACK;
		}

		void RemoveNode(Pointer node) {
			Pointer replacement_node;
			Pointer fixup_node;
			Color   original_color = node->m_color;

			if (node->m_left == nullptr) {
				fixup_node = node->m_right;
				Transplant(node, node->m_right);
			} else if (node->m_right == nullptr) {
				fixup_node = node->m_left;
				Transplant(node, node->m_left);
			} else {
				replacement_node = Minimum(node->m_right);
				original_color   = replacement_node->m_color;
				fixup_node       = replacement_node->m_right;

				if (replacement_node->m_parent == node) { if (fixup_node) { fixup_node->m_parent = replacement_node; } } else {
					Transplant(replacement_node, replacement_node->m_right);
					replacement_node->m_right           = node->m_right;
					replacement_node->m_right->m_parent = replacement_node;
				}

				Transplant(node, replacement_node);
				replacement_node->m_left           = node->m_left;
				replacement_node->m_left->m_parent = replacement_node;
				replacement_node->m_color          = node->m_color;
			}

			delete node;

			if (original_color == Color::BLACK) { FixRemoval(fixup_node); }
		}

		void FixRemoval(Pointer node) {
			while (node != m_root && (node == nullptr || node->m_color == Color::BLACK)) {
				if (node == node->m_parent->m_left) {
					Pointer sibling = node->m_parent->m_right;
					if (sibling->m_color == Color::RED) {
						sibling->m_color        = Color::BLACK;
						node->m_parent->m_color = Color::RED;
						RotateLeft(node->m_parent);
						sibling = node->m_parent->m_right;
					}
					if ((sibling->m_left == nullptr || sibling->m_left->m_color == Color::BLACK) &&
					    (sibling->m_right == nullptr || sibling->m_right->m_color == Color::BLACK)) {
						sibling->m_color = Color::RED;
						node             = node->m_parent;
					} else {
						if (sibling->m_right == nullptr || sibling->m_right->m_color == Color::BLACK) {
							if (sibling->m_left) { sibling->m_left->m_color = Color::BLACK; }
							sibling->m_color = Color::RED;
							RotateRight(sibling);
							sibling = node->m_parent->m_right;
						}
						sibling->m_color        = node->m_parent->m_color;
						node->m_parent->m_color = Color::BLACK;
						if (sibling->m_right) { sibling->m_right->m_color = Color::BLACK; }
						RotateLeft(node->m_parent);
						node = m_root;
					}
				} else {
					Pointer sibling = node->m_parent->m_left;
					if (sibling->m_color == Color::RED) {
						sibling->m_color        = Color::BLACK;
						node->m_parent->m_color = Color::RED;
						RotateRight(node->m_parent);
						sibling = node->m_parent->m_left;
					}
					if ((sibling->m_right == nullptr || sibling->m_right->m_color == Color::BLACK) &&
					    (sibling->m_left == nullptr || sibling->m_left->m_color == Color::BLACK)) {
						sibling->m_color = Color::RED;
						node             = node->m_parent;
					} else {
						if (sibling->m_left == nullptr || sibling->m_left->m_color == Color::BLACK) {
							if (sibling->m_right) { sibling->m_right->m_color = Color::BLACK; }
							sibling->m_color = Color::RED;
							RotateLeft(sibling);
							sibling = node->m_parent->m_left;
						}
						sibling->m_color        = node->m_parent->m_color;
						node->m_parent->m_color = Color::BLACK;
						if (sibling->m_left) { sibling->m_left->m_color = Color::BLACK; }
						RotateRight(node->m_parent);
						node = m_root;
					}
				}
			}
			if (node) { node->m_color = Color::BLACK; }
		}

		void Transplant(Pointer u, Pointer v) {
			if (u->m_parent == nullptr) { m_root = v; } else
				if (u == u->m_parent->m_left) { u->m_parent->m_left = v; } else { u->m_parent->m_right = v; }
			if (v != nullptr) { v->m_parent = u->m_parent; }
		}

		Pointer Minimum(Pointer node) const {
			while (node->m_left != nullptr) { node = node->m_left; }
			return node;
		}

		Pointer Maximum(Pointer node) const {
			while (node->m_right != nullptr) { node = node->m_right; }
			return node;
		}

		Pointer NextNode(Pointer node) const {
			if (node->m_right != nullptr) { return Minimum(node->m_right); }
			Pointer parent = node->m_parent;
			while (parent != nullptr && node == parent->m_right) {
				node   = parent;
				parent = parent->m_parent;
			}
			return parent;
		}

		Pointer PrevNode(Pointer node) const {
			if (node->m_left != nullptr) { return Maximum(node->m_left); }
			Pointer parent = node->m_parent;
			while (parent != nullptr && node == parent->m_left) {
				node   = parent;
				parent = parent->m_parent;
			}
			return parent;
		}

		void DeleteTree(Pointer node) {
			if (node != nullptr) {
				DeleteTree(node->m_left);
				DeleteTree(node->m_right);
				delete node;
			}
		}

		Pointer m_root;
		size_t  m_size;
		Compare m_compare;
	};

	template<typename Key, typename Value, typename Compare = Less<Key>> void Swap(Map<Key, Value, Compare>& lhs, Map<Key, Value, Compare>& rhs) { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_MAP_HPP