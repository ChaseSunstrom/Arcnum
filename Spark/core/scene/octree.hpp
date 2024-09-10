#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include <core/ecs/components/transform_component.hpp>
#include <core/event/event.hpp>
#include <core/pch.hpp>
#include <include/glm/vec3.hpp>

namespace Spark {
	struct PointData {
		Math::Vec3 position;
		u32       entity_id;
	};

	class Octree {
	  public:
		Octree(Math::Vec3 center, f32 width);
		// On components added, removed, or deleted
		void                   OnEvent(const SharedPtr<ComponentEvent<TransformComponent>> event);
		void                   Insert(const PointData& point_data);
		bool                   Remove(u32 entity_id);
		bool                   Update(const PointData& point_data);
		std::vector<PointData> QueryRange(const Math::Vec3& min, const Math::Vec3& max);
		void                   Grow(const Math::Vec3& new_point);
		void                   Shrink();
		void                   Rebuild();

		// Gets the list of all entities in the same octree node as the point
		std::vector<u32> GetEntitiesFromPoint(const Math::Vec3& point);

	  private:
		struct Node {
			Node(Math::Vec3 c, f32 hw)
				: center(c)
				, half_width(hw) {}
			Math::Vec3                            center;
			f32                                  half_width;
			std::vector<PointData>               points;
			std::array<std::unique_ptr<Node>, 8> children;
		};

		Node* Insert(Node* node, const PointData& point_data, i32 depth);
		void  CollectAllPoints(const Node* node, std::vector<PointData>& all_points) const;
		bool  IsPointInBounds(const Math::Vec3& point, const Math::Vec3& center, f32 half_width) const;
		void  QueryRange(Node* node, const Math::Vec3& min, const Math::Vec3& max, std::vector<PointData>& result);
		bool  IntersectsRange(const Math::Vec3& center, f32 half_width, const Math::Vec3& min, const Math::Vec3& max);
		bool  IsPointInRange(const Math::Vec3& point, const Math::Vec3& min, const Math::Vec3& max);

	  private:
		static inline const i32        MAX_POINTS = 4;
		static inline const i32        MAX_DEPTH  = 8;
		static inline const f32        MAX_SIZE   = 1000000.0f; // Maximum size limit
		std::unique_ptr<Node>          m_root;
		std::unordered_map<i64, Node*> m_entity_to_node;
	};

} // namespace Spark

#endif // SPARK_OCTREE_HPP