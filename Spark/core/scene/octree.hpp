#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include <core/pch.hpp>
#include <include/glm/vec3.hpp>

namespace Spark
{
    struct PointData
    {
        glm::vec3 position;
        i64 entity_id;
    };

    class Octree
    {
    public:
        Octree(glm::vec3 center, f32 width);
        void Insert(const PointData& point_data);
        bool Remove(i64 entity_id);
        bool Update(i64 entity_id, const glm::vec3& new_position);
        std::vector<PointData> QueryRange(const glm::vec3& min, const glm::vec3& max);
        void Grow(const glm::vec3& new_point);
        void Shrink();
        void Rebuild();

    private:
        struct Node
        {
            Node(glm::vec3 c, f32 hw) : center(c), half_width(hw) {}
            glm::vec3 center;
            f32 half_width;
            std::vector<PointData> points;
            std::array<std::unique_ptr<Node>, 8> children;
        };

        Node* Insert(Node* node, const PointData& point_data, i32 depth);
        void CollectAllPoints(const Node* node, std::vector<PointData>& all_points) const;
        bool IsPointInBounds(const glm::vec3& point, const glm::vec3& center, f32 half_width) const;
        void QueryRange(Node* node, const glm::vec3& min, const glm::vec3& max, std::vector<PointData>& result);
        bool IntersectsRange(const glm::vec3& center, f32 half_width, const glm::vec3& min, const glm::vec3& max);
        bool IsPointInRange(const glm::vec3& point, const glm::vec3& min, const glm::vec3& max);
    private:
        static const i32 MAX_POINTS = 4;
        static const i32 MAX_DEPTH = 8;
        static const f32 MAX_SIZE;  // Maximum size limit
        std::unique_ptr<Node> m_root;
        std::unordered_map<i64, Node*> m_entity_to_node;
    };

    const f32 Octree::MAX_SIZE = 1000000.0f;
}

#endif // SPARK_OCTREE_HPP