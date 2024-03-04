#ifndef SPARK_INSTANCER_HPP
#define SPARK_INSTANCER_HPP

#include "../spark.hpp"

#include "../ecs/component/component_types.hpp"

namespace spark
{
	class instancer
	{
	public:
		instancer()
		{
			generate_vertex_buffer(instance_vbo);
		}

		~instancer()
		{
			delete_vertex_buffer(instance_vbo);
		}

        const std::vector<math::mat4>& transforms_to_matricies(const std::vector<transform_component>& transform_array)
        {
            std::vector<math::mat4> matrices;
			matrices.reserve(transform_array.size());

            for (const auto& transform : transform_array)
            {
				matrices.push_back(transform.to_mat4());
            }

			return matrices;
        }

        void bind(const std::vector<transform_component>& transform_array)
        {
            // Convert transform components into a vector of mat4
			const std::vector<math::mat4>& matrices = transforms_to_matricies(transform_array);
            
            glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
            glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(math::mat4), matrices.data(), GL_DYNAMIC_DRAW);

            GLsizei vec4Size = sizeof(math::vec4);
            for (int32_t i = 0; i < 4; i++)
            {
                glEnableVertexAttribArray(3 + i);
                glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));
                glVertexAttribDivisor(3 + i, 1);
            }
        }

	private:
		GLuint instance_vbo;
	};
}

#endif