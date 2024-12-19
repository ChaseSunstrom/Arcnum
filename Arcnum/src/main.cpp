#include <core/render/gl/gl_material.hpp>
#include <core/render/gl/gl_mesh.hpp>
#include <core/render/gl/gl_renderer.hpp>
#include <core/render/gl/gl_shader.hpp>
#include <core/spark.hpp>
#include <core/render/camera.hpp>
#include <core/window/gl/gl_window.hpp>

// Mouse input system
auto MouseSystem = [](Spark::Application& app, const Spark::MultiEventPtr<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>& event) {
	if (event->Is<Spark::MouseButtonPressedEvent>()) {
		auto pressed_event = event->Get<Spark::MouseButtonPressedEvent>();
		LOG_INFO("Mouse Pressed: " << pressed_event->button);
	} else if (event->Is<Spark::MouseButtonReleasedEvent>()) {
		auto released_event = event->Get<Spark::MouseButtonReleasedEvent>();
		LOG_INFO("Mouse Released: " << released_event->button);
	}
};

// Scene setup system
void SetupScene(Spark::Application& app) {
	// Create shader
	const char* vertexSrc   = R"(
        #version 450 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec3 a_Normal;
        layout(location = 2) in vec2 a_TexCoord;
        layout(location = 3) in mat4 a_Transform;

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

	auto& shader            = app.CreateAsset<Spark::GLShader>("basic");
	shader.AddStage(Spark::ShaderStage::VERTEX, vertexSrc);
	shader.AddStage(Spark::ShaderStage::FRAGMENT, fragmentSrc);
	shader.Compile();

	// Create mesh
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

	auto& mesh                 = app.CreateAsset<Spark::GLGenericMesh>("triangle", Spark::BufferUsage::STATIC_DRAW);
	mesh.SetVertexData(vertices, layout);
	mesh.SetIndexData(indices);

	// Create material
	auto& material = app.CreateAsset<Spark::GLMaterial>("triangle_material");
	material.SetShader(app.GetAsset<Spark::GLShader>("basic"));
	material.SetParameter("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Set up instance data
	Spark::VertexLayout instance_layout;
	instance_layout.AddAttribute<glm::mat4>("a_Transform", Spark::AttributeType::MAT4);

	Spark::Vector<glm::mat4> transforms;
	for (i32 i = 0; i < 5; ++i) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f - 2.0f, 0.0f, 0.0f));
		transforms.PushBack(transform);
	}

	mesh.SetInstanceData(transforms, instance_layout);

	app.CreateAsset<Spark::Camera>("main");
}

// Render system
void RenderSystem(Spark::Application& app) {

	auto& material = app.GetAsset<Spark::GLMaterial>("triangle_material");
	auto& camera   = app.GetAsset<Spark::Camera>("main");
	auto& mesh     = app.GetAsset<Spark::GLGenericMesh>("triangle");
	auto& renderer = app.GetRenderer();

	material.SetParameter("u_ViewProjection", camera.GetProjectionMatrix() * camera.GetViewMatrix());

	auto cmd = Spark::Renderer::CreateInstancedDrawCommand(&mesh, &material, 5);
	renderer.Submit(cmd);
};

int main() {
	Spark::Application app(Spark::GraphicsAPI::OPENGL);

	// Setup window and renderer
	app.CreateWindow<Spark::GLWindow>("Generic Renderer Demo", 1920 / 2, 1080 / 2).CreateRenderer<Spark::GLRenderer>();

	// Add scene setup system
	app.AddSystem([](Spark::Application& app) { SetupScene(app); }, Spark::SystemInfo{.stage = Spark::SystemStage::First});

	// Add render system
	app.AddSystem([](Spark::Application& app) { RenderSystem(app); }, Spark::SystemInfo{.stage = Spark::SystemStage::Render});

	// Add input system (without explicit template parameter)
	app.AddSystem(
		[](Spark::Application& app, const Spark::MultiEventPtr<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>& event) {
			if (event->Is<Spark::MouseButtonPressedEvent>()) {
				auto pressed_event = event->Get<Spark::MouseButtonPressedEvent>();
				LOG_INFO("Mouse Pressed: " << pressed_event->button);
			} else if (event->Is<Spark::MouseButtonReleasedEvent>()) {
				auto released_event = event->Get<Spark::MouseButtonReleasedEvent>();
				LOG_INFO("Mouse Released: " << released_event->button);
			}
		},
		Spark::SystemInfo{.stage = Spark::SystemStage::PreUpdate});

	// Add resource
	app.AddResource(Spark::FPSResource{});

	// Add FPS system (without explicit template parameter)
	app.AddSystem(
		[](Spark::Application& app, Spark::ResMut<Spark::FPSResource> fps) {
			// Assuming there's an Update method in FPSResource
			fps->calculator.Update();
		},
		Spark::SystemInfo{.stage = Spark::SystemStage::PostUpdate});

	app.Start();
	return 0;
}