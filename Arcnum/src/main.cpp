#include <core/render/gl/gl_renderer.hpp>
#include <core/spark.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/render/gl/gl_shader.hpp>
#include <core/render/gl/gl_mesh.hpp>
#include <core/render/gl/gl_material.hpp>

void MouseEvents(Spark::Application& app, const Spark::MultiEventPtr<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>& event) {
	if (event->Is<Spark::MouseButtonPressedEvent>()) {
		auto pressed_event = event->Get<Spark::MouseButtonPressedEvent>();
		LOG_INFO("Mouse Pressed: " << pressed_event->button);
	} else if (event->Is<Spark::MouseButtonReleasedEvent>()) {
		auto released_event = event->Get<Spark::MouseButtonReleasedEvent>();
		LOG_INFO("Mouse Released: " << released_event->button);
	}
}

void CreateShader(Spark::Application& app) {
	const char* vertexSrc   = R"(
        #version 450 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec3 a_Normal;
        layout(location = 2) in vec2 a_TexCoord;
        layout(location = 3) in mat4 a_Transform; // Instance transform

        uniform mat4 u_ViewProjection;

        void main() {
            gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
        }
    )";

	const char* fragmentSrc = R"(
        #version 450 core
        uniform vec4 u_Color;
        out vec4 FragColor;

        void main() {
            FragColor = u_Color;
        }
    )";

	auto& shader_manager    = app.GetManager<Spark::Shader>();
	auto  shader            = shader_manager.Create<Spark::GLShader>("basic");
	shader->AddStage(Spark::ShaderStage::VERTEX, vertexSrc);
	shader->AddStage(Spark::ShaderStage::FRAGMENT, fragmentSrc);
	shader->Compile();
}

void CreateTriangleMesh(Spark::Application& app) {
	auto& mesh_manager = app.GetManager<Spark::GenericMesh>();

	// Define vertex layout
	Spark::VertexLayout layout;
	layout.AddAttribute<glm::vec3>("a_Position", Spark::AttributeType::VEC3);
	layout.AddAttribute<glm::vec3>("a_Normal", Spark::AttributeType::VEC3);
	layout.AddAttribute<glm::vec2>("a_TexCoord", Spark::AttributeType::VEC2);

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	Spark::Vector<Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
		{ {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{  {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
    };

	Spark::Vector<u32> indices = {0, 1, 2};

	auto mesh                  = mesh_manager.Create<Spark::GLGenericMesh>("triangle", Spark::BufferUsage::STATIC_DRAW);
	mesh->SetVertexData(vertices, layout);
	mesh->SetIndexData(indices);
}

void CreateMaterial(Spark::Application& app) {
	auto& material_manager = app.GetManager<Spark::Material>();
	auto  shader           = app.GetManager<Spark::Shader>().Get("basic");

	auto material          = material_manager.Create<Spark::GLMaterial>("triangle_material");
	material->SetShader(shader);
	material->SetParameter("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void SetupTestScene(Spark::Application& app) {
	CreateShader(app);
	CreateTriangleMesh(app);
	CreateMaterial(app);

	// Set up instance data
	auto mesh = app.GetManager<Spark::GenericMesh>().Get("triangle");

	Spark::VertexLayout instance_layout;
	instance_layout.AddAttribute<glm::mat4>("a_Transform", Spark::AttributeType::MAT4);

	Spark::Vector<glm::mat4> transforms;
	for (i32 i = 0; i < 5; ++i) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f - 2.0f, 0.0f, 0.0f));
		transforms.PushBack(transform);
	}

	mesh->SetInstanceData(transforms, instance_layout);
}

void RenderScene(Spark::Application& app) {
	auto renderer = static_cast<Spark::GLRenderer*>(&app.GetRenderer());
	auto mesh     = app.GetManager<Spark::GenericMesh>().Get("triangle");
	auto material = app.GetManager<Spark::Material>().Get("triangle_material");

	// Update view-projection matrix
	auto camera   = app.GetManager<Spark::Camera>().GetCurrentCamera();
	material->SetParameter("u_ViewProjection", camera->GetProjectionMatrix() * camera->GetViewMatrix());

	// Draw
	auto cmd = Spark::Renderer::CreateInstancedDrawCommand(mesh, material, 5);
	renderer->Submit(cmd);
}

void UpdateCamera(Spark::Application& app) {
	auto cam = app.GetManager<Spark::Camera>().GetCurrentCamera();
	cam->SetFar(100000.0f);
}

int main() {
	Spark::Application app(Spark::GraphicsAPI::OPENGL);
	app.CreateWindow<Spark::GLWindow>("Generic Renderer Demo", 1920 / 2, 1080 / 2)
		.CreateRenderer<Spark::GLRenderer>()
		.AddSystem<Spark::FPSSystem>()
		.AddStartupFunction(SetupTestScene)
		.AddUpdateFunction(RenderScene) // Add render function to update loop
		.AddUpdateFunction(UpdateCamera)
		.AddEventFunction<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>(MouseEvents)
		.Start();
}