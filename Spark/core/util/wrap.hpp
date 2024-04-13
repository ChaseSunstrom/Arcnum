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

	void set_background_color(f64 r, f64 g, f64 b, f64 a);

	void clear_screen();

	void set_viewport(i32 x, i32 y, i32 width, i32 height);

	void set_depth_test(bool enabled);

	void set_blending(bool enabled);

	void use_shader_program(GLuint program);

	void bind_vertex_array(GLuint vao);

	void bind_vertex_buffer(u32 vbo);

	void bind_index_buffer(u32 ibo);

	void draw_elements(u32 mode, i32 count, u32 type, const void* indices);

	void set_vertex_attribute_ptr(u32 index, i32 size, u32 type, i32 stride, const void* pointer);
	
	void enable_vertex_attribute_ptr(u32 index);

	void generate_vertex_array(u32& vao);

	void generate_vertex_buffer(u32& vbo);

	void generate_index_buffer(u32& ibo);

	void delete_vertex_array(u32& vao);

	void delete_vertex_buffer(u32& vbo);

	void delete_index_buffer(u32& ibo);

	void buffer_index_data(u32 ibo, const std::vector <u32>& data);

	void buffer_index_subdata(u32 ibo, const std::vector <u32>& data);

	GLuint create_program();

	void delete_program(u32 program);
	
	void delete_shader(u32 shader);

	void attach_shader(u32 program, u32 shader);

	void link_program(u32 program);

	void use_program(u32 program);

	GLuint create_shader(u32 type);

	void shader_source(u32 shader, const std::string& source);

	void compile_shader(u32 shader);

	void set_uniform(const std::string& name, const i32 value, GLuint shader_program);

	void set_uniform(const std::string& name, const f32 value, GLuint shader_program);

	void set_uniform(const std::string& name, const f64 value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::mat2& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::mat3& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::mat4& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::vec2& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::vec3& value, GLuint shader_program);

	void set_uniform(const std::string& name, const math::vec4& value, GLuint shader_program);

	void set_uniform(const std::string& name, const bool value, GLuint shader_program);

	void generate_mipmap(GLenum target);

	void active_texture(GLenum texture);

	void generate_texture(u32& texture);

	void bind_texture(GLenum target, u32 texture);
	
	template <typename T>
	void buffer_vertex_data(u32 vbo, const std::vector <T>& data)
	{
		bind_vertex_buffer(vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);

		check_gl_error("Buffer vertex data");
	}

	template <typename T>
	void buffer_vertex_subdata(u32 vbo, const std::vector <T>& data)
	{
		bind_vertex_buffer(vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(T), data.data());

		check_gl_error("Buffer sub data");
	}
}

#endif