#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include <core/ecs/components/transform_component.hpp>
#include <core/event/event.hpp>
#include <core/pch.hpp>
#include <include/glm/vec3.hpp>

namespace Spark {
	struct PointData {
		_MATH Vec3 position;
		u32       entity_id;
	};

	class Octree {
	  public:
		Octree(_MATH Vec3 center, f32 width);
		// On components added, removed, or deleted
		void                   OnEvent(const SharedPtr<ComponentEvent<TransformComponent>> event);
		void                   Insert(const PointData& point_data);
		bool                   Remove(u32 entity_id);
		bool                   Update(const PointData& point_data);
		std::vector<PointData> QueryRange(const _MATH Vec3& min, const _MATH Vec3& max);
		void                   Grow(const _MATH Vec3& new_point);
		void                   Shrink();
		void                   Rebuild();

		// Gets the list of all entities in the same octree node as the point
		std::vector<u32> GetEntitiesFromPoint(const _MATH Vec3& point);

	  private:
		struct Node {
			Node(_MATH Vec3 c, f32 hw)
				: center(c)
				, half_width(hw) {}
			_MATH Vec3                            center;
			f32                                  half_width;
			std::vector<PointData>               points;
			std::array<std::unique_ptr<Node>, 8> children;
		};

		Node* Insert(Node* node, const PointData& point_data, i32 depth);
		void  CollectAllPoints(const Node* node, std::vector<PointData>& all_points) const;
		bool  IsPointInBounds(const _MATH Vec3& point, const _MATH Vec3& center, f32 half_width) const;
		void  QueryRange(Node* node, const _MATH Vec3& min, const _MATH Vec3& max, std::vector<PointData>& result);
		bool  IntersectsRange(const _MATH Vec3& center, f32 half_width, const _MATH Vec3& min, const _MATH Vec3& max);
		bool  IsPointInRange(const _MATH Vec3& point, const _MATH Vec3& min, const _MATH Vec3& max);

	  private:
		static inline const i32        MAX_POINTS = 4;
		static inline const i32        MAX_DEPTH  = 8;
		static inline const f32        MAX_SIZE   = 1000000.0f; // Maximum size limit
		std::unique_ptr<Node>          m_root;
		std::unordered_map<i64, Node*> m_entity_to_node;
	};

} // namespace Spark

#endif // SPARK_OCTREE_HPP