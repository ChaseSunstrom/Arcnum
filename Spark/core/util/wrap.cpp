#include "wrap.hpp"

namespace spark
{
	void set_background_color(float64_t r, float64_t g, float64_t b, float64_t a)
	{
		glClearColor(r, g, b, a);

		check_gl_error("Background color set");
	}

	void clear_screen()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		check_gl_error("Screen clear");
	}

	void set_viewport(int32_t x, int32_t y, int32_t width, int32_t height)
	{
		glViewport(x, y, width, height);

		check_gl_error("Viewport set");
	}

	void set_depth_test(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_DEPTH_TEST);
		}

		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		check_gl_error("Depth test set");
	}

	void set_blending(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		else
		{
			glDisable(GL_BLEND);
		}

		check_gl_error("Blending set");
	}

	void use_shader_program(GLuint program)
	{
		glUseProgram(program);

		check_gl_error("Program use");
	}

	void bind_vertex_array(GLuint vao)
	{
		glBindVertexArray(vao);

		check_gl_error("Vertex array bind");
	}

	void bind_vertex_buffer(uint32_t vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		check_gl_error("Vertex buffer bind");
	}

	void bind_index_buffer(uint32_t ibo)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		check_gl_error("Index buffer bind");
	}

	void draw_elements(uint32_t mode, int32_t count, uint32_t type, const void* indices)
	{
		glDrawElements(mode, count, type, indices);

		check_gl_error("Draw elements");
	}

	void set_vertex_attribute_ptr(
		uint32_t index, int32_t size, uint32_t type, int32_t stride, const void* pointer)
	{
		glVertexAttribPointer(index, size, type, GL_FALSE, stride, pointer);

		check_gl_error("Vertex attribute set");
	}

	void enable_vertex_attribute_ptr(uint32_t index)
	{
		glEnableVertexAttribArray(index);

		check_gl_error("Vertex attribute enable");
	}

	void generate_vertex_array(uint32_t& vao)
	{
		glGenVertexArrays(1, &vao);

		check_gl_error("Vertex array generate");
	}

	void generate_vertex_buffer(uint32_t& vbo)
	{
		glGenBuffers(1, &vbo);

		check_gl_error("Vertex buffer generate");
	}

	void generate_index_buffer(uint32_t& ibo)
	{
		glGenBuffers(1, &ibo);

		check_gl_error("Index buffer generate");
	}

	void delete_vertex_array(uint32_t& vao)
	{
		glDeleteVertexArrays(1, &vao);

		check_gl_error("Vertex array delete");
	}

	void delete_vertex_buffer(uint32_t& vbo)
	{
		glDeleteBuffers(1, &vbo);

		check_gl_error("Vertex buffer delete");
	}

	void delete_index_buffer(uint32_t& ibo)
	{
		glDeleteBuffers(1, &ibo);

		check_gl_error("Index buffer delete");
	}

	void buffer_index_data(uint32_t ibo, const std::vector <uint32_t>& data)
	{
		bind_index_buffer(ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof GLuint, data.data(), GL_DYNAMIC_DRAW);

		check_gl_error("Index data in buffer");
	}

	void buffer_index_subdata(uint32_t ibo, const std::vector <uint32_t>& data)
	{
		bind_vertex_buffer(ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.size() * sizeof GLuint, data.data());

		check_gl_error("Index data in buffer");
	}

	GLuint create_program()
	{
		GLuint program = glCreateProgram();

		check_gl_error("Program create");

		return program;
	}

	void delete_program(uint32_t program)
	{
		glDeleteProgram(program);

		check_gl_error("Program delete");
	}

	void delete_shader(uint32_t shader)
	{
		glDeleteShader(shader);

		check_gl_error("Shader delete");
	}

	void attach_shader(uint32_t program, uint32_t shader)
	{
		glAttachShader(program, shader);

		check_gl_error("Shader attach");
	}

	void link_program(uint32_t program)
	{
		glLinkProgram(program);

		GLint success;
		GLchar infoLog[1024];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 1024, NULL, infoLog);
			SPARK_ERROR("[SHADER]: Linking error \n" << infoLog << "\n -- --------------------------------------------------- -- ");
		}

		check_gl_error("Program link");
	}

	void use_program(uint32_t program)
	{
		glUseProgram(program);

		check_gl_error("Program use");
	}

	GLuint create_shader(uint32_t type)
	{
		GLuint shader = glCreateShader(type);

		check_gl_error("Shader create");

		return shader;
	}

	void shader_source(uint32_t shader, const std::string& source)
	{
		const char* source_ptr = source.c_str();
		glShaderSource(shader, 1, &source_ptr, nullptr);

		check_gl_error("Shader source");
	}

	void compile_shader(uint32_t shader)
	{
		glCompileShader(shader);

		GLint success;
		GLchar infoLog[1024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			SPARK_ERROR("[SHADER]: Compilation error of type: " << "\n" << infoLog << "\n -- --------------------------------------------------- -- ");
		}

		check_gl_error("Shader compilation");
	}

	void set_uniform(const std::string& name, const int32_t value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1i(loc, value);
	}

	void set_uniform(const std::string& name, const float32_t value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1f(loc, value);
	}

	void set_uniform(const std::string& name, const float64_t value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1d(loc, value);
	}

	void set_uniform(const std::string& name, const math::mat2& value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniformMatrix2fv(loc, 1, GL_FALSE, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::mat3& value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::mat4& value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::vec2& value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform2fv(loc, 1, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::vec3& value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform3fv(loc, 1, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::vec4& value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform4fv(loc, 1, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const bool value, GLuint shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1i(loc, value ? 1 : 0);
	}

	void generate_mipmap(GLenum target)
	{
		glGenerateMipmap(target);
	}

	void active_texture(GLenum texture)
	{
		glActiveTexture(texture);
	}

	void generate_texture(uint32_t& texture)
	{
		glGenTextures(1, &texture);
	}

	void bind_texture(GLenum target, uint32_t texture)
	{
		glBindTexture(target, texture);
	}

}