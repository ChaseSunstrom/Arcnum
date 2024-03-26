#ifndef SPARK_WRAP_HPP
#define SPARK_WRAP_HPP

#include "../spark.hpp"

#include "../logging/log.hpp"

// ===============================================================================
// MAIN FILE FOR WRAPPING OPENGL CALLS
// ===============================================================================

namespace spark
{

#ifdef __SPARK_DEBUG__

#define check_gl_error(_type) \
	do { \
        GLenum err; \
        while ((err = glGetError()) != GL_NO_ERROR) { \
            std::string error; \
            switch (err) { \
                case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break; \
                case GL_INVALID_ENUM:           error="INVALID_ENUM";           break; \
                case GL_INVALID_VALUE:          error="INVALID_VALUE";          break; \
                case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break; \
                case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break; \
            } \
            SPARK_ERROR("[" << _type << "]: OpenGL error - " << error); \
        } \
    } while (0)
#else

#define check_gl_error(_type)

#endif

	void set_background_color(float64_t r, float64_t g, float64_t b, float64_t a);

	void clear_screen();

	void set_viewport(int32_t x, int32_t y, int32_t width, int32_t height);

	void set_depth_test(bool enabled);

	void set_blending(bool enabled);

	void use_shader_program(GLuint program);

	void bind_vertex_array(GLuint vao);

	void bind_vertex_buffer(uint32_t vbo);

	void bind_index_buffer(uint32_t ibo);

	void draw_elements(uint32_t mode, int32_t count, uint32_t type, const void* indices);

	void set_vertex_attribute_ptr(
		uint32_t index, int32_t size, uint32_t type, int32_t stride, const void* pointer);
	
	void enable_vertex_attribute_ptr(uint32_t index);

	void generate_vertex_array(uint32_t& vao);

	void generate_vertex_buffer(uint32_t& vbo);

	void generate_index_buffer(uint32_t& ibo);

	void delete_vertex_array(uint32_t& vao);

	void delete_vertex_buffer(uint32_t& vbo);

	void delete_index_buffer(uint32_t& ibo);

	void buffer_index_data(uint32_t ibo, const std::vector <uint32_t>& data);

	void buffer_index_subdata(uint32_t ibo, const std::vector <uint32_t>& data);

	GLuint create_program();

	void delete_program(uint32_t program);
	
	void delete_shader(uint32_t shader);

	void attach_shader(uint32_t program, uint32_t shader);

	void link_program(uint32_t program);

	void use_program(uint32_t program);

	GLuint create_shader(uint32_t type);

	void shader_source(uint32_t shader, const std::string& source);

	void compile_shader(uint32_t shader);

	void set_uniform(const std::string& name, const int32_t value, GLuint shader_program);

	void set_uniform(const std::string& name, const float32_t value, GLuint shader_program);

	void set_uniform(const std::string& name, const float64_t value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::mat2& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::mat3& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::mat4& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::vec2& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::vec3& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::vec4& value, GLuint shader_program);

	void set_uniform(const std::string& name, const bool value, GLuint shader_program);

	void generate_mipmap(GLenum target);

	void active_texture(GLenum texture);

	void generate_texture(uint32_t& texture);

	void bind_texture(GLenum target, uint32_t texture);
	
	template <typename T>
	void buffer_vertex_data(uint32_t vbo, const std::vector <T>& data)
	{
		bind_vertex_buffer(vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);

		check_gl_error("Buffer vertex data");
	}

	template <typename T>
	void buffer_vertex_subdata(uint32_t vbo, const std::vector <T>& data)
	{
		bind_vertex_buffer(vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(T), data.data());

		check_gl_error("Buffer sub data");
	}
}

#endif