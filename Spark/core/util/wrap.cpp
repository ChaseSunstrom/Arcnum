#include "wrap.hpp"

namespace spark
{
	void set_background_color(f64 r, f64 g, f64 b, f64 a)
	{
		glClearColor(r, g, b, a);

		check_gl_error("Background color set");
	}

	void clear_screen()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		check_gl_error("Screen clear");
	}

	void set_viewport(i32 x, i32 y, i32 width, i32 height)
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

	void use_shader_program(u32 program)
	{
		glUseProgram(program);

		check_gl_error("Program use");
	}

	void bind_vertex_array(u32 vao)
	{
		glBindVertexArray(vao);

		check_gl_error("Vertex array bind");
	}

	void bind_vertex_buffer(u32 vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		check_gl_error("Vertex buffer bind");
	}

	void bind_index_buffer(u32 ibo)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		check_gl_error("Index buffer bind");
	}

	void draw_elements(u32 mode, i32 count, u32 type, const void* indices)
	{
		glDrawElements(mode, count, type, indices);

		check_gl_error("Draw elements");
	}

	void set_vertex_attribute_ptr(
			u32 index, i32 size, u32 type, i32 stride, const void* pointer)
	{
		glVertexAttribPointer(index, size, type, GL_FALSE, stride, pointer);

		check_gl_error("Vertex attribute set");
	}

	void enable_vertex_attribute_ptr(u32 index)
	{
		glEnableVertexAttribArray(index);

		check_gl_error("Vertex attribute enable");
	}

	void generate_vertex_array(u32& vao)
	{
		glGenVertexArrays(1, &vao);

		check_gl_error("Vertex array generate");
	}

	void generate_vertex_buffer(u32& vbo)
	{
		glGenBuffers(1, &vbo);

		check_gl_error("Vertex buffer generate");
	}

	void generate_index_buffer(u32& ibo)
	{
		glGenBuffers(1, &ibo);

		check_gl_error("Index buffer generate");
	}

	void delete_vertex_array(u32& vao)
	{
		glDeleteVertexArrays(1, &vao);

		check_gl_error("Vertex array delete");
	}

	void delete_vertex_buffer(u32& vbo)
	{
		glDeleteBuffers(1, &vbo);

		check_gl_error("Vertex buffer delete");
	}

	void delete_index_buffer(u32& ibo)
	{
		glDeleteBuffers(1, &ibo);

		check_gl_error("Index buffer delete");
	}

	void buffer_index_data(u32 ibo, const std::vector <u32>& data)
	{
		bind_index_buffer(ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof u32, data.data(), GL_DYNAMIC_DRAW);

		check_gl_error("Index data in buffer");
	}

	void buffer_index_subdata(u32 ibo, const std::vector <u32>& data)
	{
		bind_vertex_buffer(ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.size() * sizeof u32, data.data());

		check_gl_error("Index data in buffer");
	}

	u32 create_program()
	{
		u32 program = glCreateProgram();

		check_gl_error("Program create");

		return program;
	}

	void delete_program(u32 program)
	{
		glDeleteProgram(program);

		check_gl_error("Program delete");
	}

	void delete_shader(u32 shader)
	{
		glDeleteShader(shader);

		check_gl_error("Shader delete");
	}

	void attach_shader(u32 program, u32 shader)
	{
		glAttachShader(program, shader);

		check_gl_error("Shader attach");
	}

	void link_program(u32 program)
	{
		glLinkProgram(program);

		GLint success;
		GLchar infoLog[1024];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 1024, NULL, infoLog);
			SPARK_ERROR("[SHADER]: Linking error \n" << infoLog
			                                         << "\n -- --------------------------------------------------- -- ");
		}

		check_gl_error("Program link");
	}

	void use_program(u32 program)
	{
		glUseProgram(program);

		check_gl_error("Program use");
	}

	u32 create_shader(u32 type)
	{
		u32 shader = glCreateShader(type);

		check_gl_error("Shader create");

		return shader;
	}

	void shader_source(u32 shader, const std::string& source)
	{
		const char* source_ptr = source.c_str();
		glShaderSource(shader, 1, &source_ptr, nullptr);

		check_gl_error("Shader source");
	}

	void compile_shader(u32 shader)
	{
		glCompileShader(shader);

		GLint success;
		GLchar infoLog[1024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			SPARK_ERROR("[SHADER]: Compilation error of type: " << "\n" << infoLog
			                                                    << "\n -- --------------------------------------------------- -- ");
		}

		check_gl_error("Shader compilation");
	}

	void set_uniform(const std::string& name, const i32 value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1i(loc, value);
	}

	void set_uniform(const std::string& name, const f32 value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1f(loc, value);
	}

	void set_uniform(const std::string& name, const f64 value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform1d(loc, value);
	}

	void set_uniform(const std::string& name, const math::mat2& value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniformMatrix2fv(loc, 1, GL_FALSE, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::mat3& value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::mat4& value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::vec2& value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform2fv(loc, 1, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::vec3& value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform3fv(loc, 1, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const math::vec4& value, u32 shader_program)
	{
		GLint loc = glGetUniformLocation(shader_program, name.c_str());
		glUniform4fv(loc, 1, math::value_ptr(value));
	}

	void set_uniform(const std::string& name, const bool value, u32 shader_program)
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

	void generate_texture(u32& texture)
	{
		glGenTextures(1, &texture);
	}

	void bind_texture(GLenum target, u32 texture)
	{
		glBindTexture(target, texture);
	}

}