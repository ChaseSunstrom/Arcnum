#include "spark_pch.hpp"
#include "opengl/spark_gl_mesh.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace spark::opengl
{
    GLMesh::GLMesh()
        : m_vao(0), m_vbo(0), m_ebo(0), m_index_count(0), m_instance_vbo(0)
    {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
        glGenBuffers(1, &m_instance_vbo);
    }

    GLMesh::~GLMesh()
    {
        glDeleteBuffers(1, &m_ebo);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_instance_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void GLMesh::SetDataBytes(const std::vector<u8>& vertex_data,
        const VertexLayout& layout,
        const std::vector<u32>& indices)
    {
        m_vertex_data = vertex_data;
        m_index_data = indices;
        m_layout = layout;
        m_index_count = indices.size();

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

        const auto& attrs = layout.GetAttributes();
        const size_t stride = layout.GetStride();

        for (size_t i = 0; i < attrs.size(); ++i)
        {
            glEnableVertexAttribArray(static_cast<GLuint>(i));

            const auto& attr = attrs[i];
            GLenum gl_type = 0;
            bool use_integer_pointer = false;

            switch (attr.type)
            {
            case AttributeType::FLOAT:
            case AttributeType::VEC2:
            case AttributeType::VEC3:
            case AttributeType::VEC4:
            case AttributeType::MAT3:
            case AttributeType::MAT4:
                gl_type = GL_FLOAT;
                use_integer_pointer = false;
                break;
            case AttributeType::INT:
            case AttributeType::IVEC2:
            case AttributeType::IVEC3:
            case AttributeType::IVEC4:
                gl_type = GL_INT;
                use_integer_pointer = true;
                break;
            default:
                break;
            }

            if (!use_integer_pointer)
            {
                glVertexAttribPointer(
                    static_cast<GLuint>(i),
                    attr.GetComponentCount(),
                    gl_type,
                    attr.normalized ? GL_TRUE : GL_FALSE,
                    static_cast<GLsizei>(stride),
                    reinterpret_cast<const void*>(attr.offset));
            }
            else
            {
                glVertexAttribIPointer(
                    static_cast<GLuint>(i),
                    attr.GetComponentCount(),
                    gl_type,
                    static_cast<GLsizei>(stride),
                    reinterpret_cast<const void*>(attr.offset));
            }
        }

        glBindVertexArray(0);
    }

    void GLMesh::SetInstanceData(const std::vector<math::Mat4>& instance_data)
    {


        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER,
            instance_data.size() * sizeof(math::Mat4),
            instance_data.data(),
            GL_DYNAMIC_DRAW);

        // We assume that the standard vertex attributes (for example, those set in SetDataBytes)
        // occupy attribute locations [0, ..., N-1]. We will use the next available locations
        // for the instance data. For a Mat4, we need 4 consecutive attribute locations.
        // For this example, let’s assume your layout has N attributes.
        GLuint base_location = static_cast<GLuint>(m_layout.GetAttributes().size());
        // A Mat4 is 16 floats. We set each column as a separate attribute.
        for (GLuint i = 0; i < 4; ++i)
        {
            GLuint location = base_location + i;
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(
                location,           // attribute location
                4,                  // 4 components (a vec4)
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                sizeof(math::Mat4), // stride between matrices
                (void*)(sizeof(float) * 4 * i)); // offset for the ith column
            // Set the divisor so that these attributes update once per instance.
            glVertexAttribDivisor(location, 1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void GLMesh::Draw() const
    {
        glBindVertexArray(m_vao);

        if (m_index_count)
        {
            glDrawElements(GL_TRIANGLES, (GLsizei)m_index_count, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, m_vertex_data.size());
        }

        glBindVertexArray(0);
    }

    void GLMesh::DrawInstanced(usize instance_count) const
    {
        glBindVertexArray(m_vao);
        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)m_index_count, GL_UNSIGNED_INT, 0, (GLsizei)instance_count);
        glBindVertexArray(0);
    }
}
