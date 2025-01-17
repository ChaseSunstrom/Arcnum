#include "spark_pch.hpp"
#include "opengl/spark_gl_mesh.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace spark
{
    GLMesh::GLMesh()
        : m_vao(0), m_vbo(0), m_ebo(0), m_index_count(0)
    {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
    }

    GLMesh::~GLMesh()
    {
        glDeleteBuffers(1, &m_ebo);
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void GLMesh::SetData(const std::vector<u8>& vertex_data,
        const VertexLayout& layout,
        const std::vector<u32>& indices)
    {
        // Store data internally if desired
        m_vertex_data = vertex_data;
        m_index_data = indices;
        m_layout = layout;
        m_index_count = indices.size();

        glBindVertexArray(m_vao);

        // Upload vertices
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

        // Upload indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

        // Setup vertex attributes
        const auto& attrs = layout.GetAttributes();
        const size_t stride = layout.GetStride();

        for (size_t i = 0; i < attrs.size(); ++i)
        {
            // Enable the vertex attribute location
            glEnableVertexAttribArray(static_cast<GLuint>(i));

            const auto& attr = attrs[i];
            // For simplicity, we'll map your enum to the appropriate GL type + function
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
                // All these are floating-point attributes
                gl_type = GL_FLOAT;
                use_integer_pointer = false;
                break;

            case AttributeType::INT:
            case AttributeType::IVEC2:
            case AttributeType::IVEC3:
            case AttributeType::IVEC4:
                // Integer attributes
                gl_type = GL_INT;
                use_integer_pointer = true;
                break;
            default:
                // fallback or assert
                break;
            }

            // Decide which function to call based on whether it's an integer attribute
            if (!use_integer_pointer)
            {
                // glVertexAttribPointer for float-based types
                glVertexAttribPointer(
                    static_cast<GLuint>(i),
                    attr.GetComponentCount(),  // e.g., 3 for VEC3
                    gl_type,                  // e.g., GL_FLOAT
                    attr.normalized ? GL_TRUE : GL_FALSE,
                    static_cast<GLsizei>(stride),
                    reinterpret_cast<const void*>(attr.offset));
            }
            else
            {
                // glVertexAttribIPointer for integer-based types
                glVertexAttribIPointer(
                    static_cast<GLuint>(i),
                    attr.GetComponentCount(),
                    gl_type,  // e.g., GL_INT
                    static_cast<GLsizei>(stride),
                    reinterpret_cast<const void*>(attr.offset));
            }
        }

        glBindVertexArray(0);
    }


    void GLMesh::Draw() const
    {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
