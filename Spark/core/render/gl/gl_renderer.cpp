#include "gl_renderer.hpp"
#include <include/glad/glad.h>

namespace Spark {
void CheckShaderCompilation(u32 shader, const char* type) {
	i32 success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
}

void CheckProgramLinking(u32 program) {
	i32 success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
}
GLRenderer::GLRenderer(GraphicsAPI gapi, Framebuffer& framebuffer)
	: Renderer(gapi, framebuffer), m_window_width(framebuffer.GetWidth()), m_window_height(framebuffer.GetHeight()) {
	f32 vertices[] = {
		-0.5f,
		-0.5f,
		0.0f, // left
		0.5f,
		-0.5f,
		0.0f, // right
		0.0f,
		0.5f,
		0.0f  // top
	};

	// Create VAO and VBO for the triangle
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*) 0);
	glEnableVertexAttribArray(0);

	// Compile shaders for the triangle
	const char* vertexShaderSource   = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";

	const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";

	u32 vertexShader        = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	CheckShaderCompilation(vertexShader, "vertex");

	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	CheckShaderCompilation(fragmentShader, "fragment");

	m_shader_program = glCreateProgram();
	glAttachShader(m_shader_program, vertexShader);
	glAttachShader(m_shader_program, fragmentShader);
	glLinkProgram(m_shader_program);
	CheckProgramLinking(m_shader_program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Create a full-screen quad for rendering the framebuffer to the screen
	f32 quadVertices[] = {
		// positions   // texCoords
		-1.0f,
		1.0f,
		0.0f,
		1.0f,
		-1.0f,
		-1.0f,
		0.0f,
		0.0f,
		1.0f,
		-1.0f,
		1.0f,
		0.0f,

		-1.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f};

	glGenVertexArrays(1, &m_quad_vao);
	glGenBuffers(1, &m_quad_vbo);
	glBindVertexArray(m_quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*) (2 * sizeof(f32)));

	// Create shader for rendering the framebuffer to the screen
	const char* screenVertexShaderSource   = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        out vec2 TexCoords;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
    )";

	const char* screenFragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
            FragColor = texture(screenTexture, TexCoords);
        }
    )";

	u32 screenVertexShader        = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
	glCompileShader(screenVertexShader);
	CheckShaderCompilation(screenVertexShader, "screen vertex");

	u32 screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(screenFragmentShader);
	CheckShaderCompilation(screenFragmentShader, "screen fragment");

	m_screen_shader_program = glCreateProgram();
	glAttachShader(m_screen_shader_program, screenVertexShader);
	glAttachShader(m_screen_shader_program, screenFragmentShader);
	glLinkProgram(m_screen_shader_program);
	CheckProgramLinking(m_screen_shader_program);

	glDeleteShader(screenVertexShader);
	glDeleteShader(screenFragmentShader);
}

GLRenderer::~GLRenderer() {
	glDeleteVertexArrays(1, &m_quad_vao);
	glDeleteBuffers(1, &m_quad_vbo);
	glDeleteProgram(m_screen_shader_program);
}

void GLRenderer::Render() {
	// First pass: Render to framebuffer
	m_window_width = m_framebuffer.GetWidth();
	m_window_height = m_framebuffer.GetHeight();

	m_framebuffer.Bind();
	glViewport(0, 0, m_framebuffer.GetWidth(), m_framebuffer.GetHeight());
	glClearColor(0.0f, 1.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shader_program);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	m_framebuffer.Unbind();

	// Second pass: Render framebuffer to screen
	RenderFramebufferToScreen();
}

void GLRenderer::RenderFramebufferToScreen() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);              // Bind the default framebuffer
	glViewport(0, 0, m_window_width, m_window_height); // Set viewport to window dimensions
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_screen_shader_program);
	glBindVertexArray(m_quad_vao);

	// Bind the framebuffer's texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer.GetTexture());
	glUniform1i(glGetUniformLocation(m_screen_shader_program, "screenTexture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
} // namespace Spark