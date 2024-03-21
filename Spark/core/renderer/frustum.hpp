#ifndef SPARK_FRUSTUM_HPP
#define SPARK_FRUSTUM_HPP

#include "../spark.hpp"
#include "../ecs/ecs.hpp"

namespace spark
{
	struct frustum
	{
		frustum() = default;
		frustum(const math::mat4& pv_matrix);
		void update(const math::mat4& view_projection_matrix);
		bool is_inside(const math::vec3& center, float32_t size) const;

		std::array<math::vec4, 6> m_planes;
	};
}

#endif