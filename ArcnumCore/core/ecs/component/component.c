#include "component.h"

#include "../../util/data/vector.h"
#include "../../util/data/file.h"

__A_CORE_API__  mesh_component* mesh_component_default(void)
{
	mesh_component* mc = ALLOC(mesh_component);
	mc->component_name = "mesh_component";
	mc->values = vector_default();
	return mc;
}

__A_CORE_API__  transform_component* transform_component_default(void)
{
	transform_component* tc = ALLOC(transform_component);
	tc->component_name = "transform_component";
	tc->position = vec3_default();
	tc->rotation = vec3_default();
	tc->scale = vec3_default();
	return tc;
}

__A_CORE_API__  shader_component* shader_component_default(void)
{
	shader_component* sc = ALLOC(shader_component);
	sc->component_name = "shader_component";
	sc->vertex_shader = 0;
	sc->fragment_shader = 0;
	sc->shader_program = glCreateProgram();
	sc->VAO = 0;
	sc->VBO = 0;
	return sc;
}

__A_CORE_API__ render_component* render_component_default(void)
{
	render_component* rc = ALLOC(render_component);
	rc->component_name = "render_component";
	rc->mesh = mesh_component_default();
	rc->shader = shader_component_default();
	rc->transform = transform_component_default();
	return rc;
}

__A_CORE_API__  texture_component* texture_component_default(void)
{
	texture_component* tc = ALLOC(texture_component);
	tc->component_name = "texture_component";
	tc->image_data = NULL;
	tc->texture = 0;
	return tc;
}

__A_CORE_API__  color_component* color_component_default(void)
{
	color_component* cc = ALLOC(color_component);
	cc->component_name = "color_component";
	cc->color = color_default();
	return cc;
}

__A_CORE_API__  mesh_component* mesh_component_new(vector(float64_t) values)
{
	mesh_component* mc = ALLOC(mesh_component);
	mc->component_name = "mesh_component";
	mc->values = values;
	return mc;
}

__A_CORE_API__  transform_component* transform_component_new(vec3 position, vec3 rotation, vec3 scale)
{
	transform_component* tc = ALLOC(transform_component);
	tc->component_name = "transform_component";
	tc->position = position;
	tc->rotation = rotation;
	tc->scale = scale;
	return tc;
}

__A_CORE_API__ shader_component* shader_component_new(vector(float64_t) vertices, c_str vertex_path, c_str fragment_path)
{
	c_str vertex_source = read_file(vertex_path);
	c_str fragment_source = read_file(fragment_path);

	shader_component* shader = ALLOC(shader_component);

	shader->component_name = "shader_component";

	shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader->vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(shader->vertex_shader);
	shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader->fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(shader->fragment_shader);

	shader->shader_program = glCreateProgram();

	glUseProgram(shader->shader_program);

	glGenVertexArrays(1, &shader->VAO);
	glBindVertexArray(shader->VAO);

	glGenBuffers(1, &shader->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, shader->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size, vertices->data, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glAttachShader(shader->shader_program, shader->vertex_shader);
	glAttachShader(shader->shader_program, shader->fragment_shader);

	glLinkProgram(shader->shader_program);

	glDeleteShader(shader->vertex_shader);
	glDeleteShader(shader->fragment_shader);

	return shader;
}

__A_CORE_API__ shader_component* _shader_component_new(c_str vertex_path, c_str fragment_path)
{
	c_str vertex_source = read_file(vertex_path);
	c_str fragment_source = read_file(fragment_path);

	shader_component* shader = ALLOC(shader_component);

	shader->component_name = "shader_component";

	shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader->vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(shader->vertex_shader);
	shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader->fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(shader->fragment_shader);

	shader->shader_program = glCreateProgram();

	glUseProgram(shader->shader_program);

	glGenVertexArrays(1, &shader->VAO);
	glBindVertexArray(shader->VAO);

	glGenBuffers(1, &shader->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, shader->VBO);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

	glAttachShader(shader->shader_program, shader->vertex_shader);
	glAttachShader(shader->shader_program, shader->fragment_shader);

	glLinkProgram(shader->shader_program);

	glDeleteShader(shader->vertex_shader);
	glDeleteShader(shader->fragment_shader);

	return shader;
}

__A_CORE_API__  render_component* render_component_new(mesh_component* mesh, shader_component* shader, transform_component* transform)
{
	render_component* rc = ALLOC(render_component);
	rc->component_name = "render_component";
	rc->mesh = mesh;
	rc->shader = shader;
	rc->transform = transform;
	return rc;
}

__A_CORE_API__  texture_component* texture_component_new(byte* image_data, GLuint texture)
{
	texture_component* tc = ALLOC(texture_component);
	tc->component_name = "texture_component";
	tc->image_data = image_data;
	tc->texture = texture;
	return tc;
}

__A_CORE_API__  color_component*	color_component_new(color color)
{
	color_component* cc = ALLOC(color_component);
	cc->component_name = "color_component";
	cc->color = color;
	return cc;
}

__A_CORE_API__ void color_on_attach(stride_type stride_size)
{
	if (stride_size == 6 || stride_size == 8)
	{
		glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, stride_size * sizeof(float64_t), 3 * sizeof(float64_t));
		glEnableVertexAttribArray(1);
	}

	else if (stride_size == 9 || stride_size == 11)
	{
		glVertexAttribPointer(2, 3, GL_DOUBLE, GL_FALSE, stride_size * sizeof(float64_t), 6 * sizeof(float64_t));
		glEnableVertexAttribArray(2);
	}
}

__A_CORE_API__ void texture_on_attach(stride_type stride_size)
{
	if (stride_size == 5)
	{
		glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, stride_size * sizeof(float64_t), stride_size - 2 * sizeof(float64_t));
		glEnableVertexAttribArray(1);
	}
	else if (stride_size == 11)
	{
		glVertexAttribPointer(3, 2, GL_DOUBLE, GL_FALSE, stride_size * sizeof(float64_t), stride_size - 2 * sizeof(float64_t));
		glEnableVertexAttribArray(3);
	}
	else
	{
		glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, stride_size * sizeof(float64_t), stride_size - 2 * sizeof(float64_t));
		glEnableVertexAttribArray(2);
	}
}

__A_CORE_API__ void normal_on_attach(stride_type stride_size)
{
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, stride_size * sizeof(float64_t), 3 * sizeof(float64_t));
	glEnableVertexAttribArray(1);
}

__A_CORE_API__ void shader_component_change_shader(shader_component* sc, c_str shader_path, GLint shader_type)
{
	if (shader_type == GL_FRAGMENT_SHADER)
	{
		c_str fragment_source = read_file(shader_path);

		sc->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sc->fragment_shader, 1, &fragment_source, NULL);
		glCompileShader(sc->fragment_shader);

		glAttachShader(sc->shader_program, sc->fragment_shader);
		glDeleteShader(sc->fragment_shader);
	}

	else if (shader_type == GL_VERTEX_SHADER)
	{

		c_str vertex_source = read_file(shader_path);

		sc->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sc->vertex_shader, 1, &vertex_source, NULL);
		glCompileShader(sc->vertex_shader);

		glAttachShader(sc->shader_program, sc->vertex_shader);
		glDeleteShader(sc->vertex_shader);
	}
}


__A_CORE_API__ void mesh_component_change_vertices(mesh_component* mc, vector(float64_t) vertices)
{
	vector_move_data(mc->values, vertices);
}