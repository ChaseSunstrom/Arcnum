#include <core/render/camera.hpp>
#include <core/render/gl/gl_material.hpp>
#include <core/render/gl/gl_mesh.hpp>
#include <core/render/gl/gl_renderer.hpp>
#include <core/render/gl/gl_shader.hpp>
#include <core/spark.hpp>
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
	layout.AddAttribute<_SPARK Vec3>("a_Position", Spark::AttributeType::VEC3);
	layout.AddAttribute<_SPARK Vec3>("a_Normal", Spark::AttributeType::VEC3);
	layout.AddAttribute<_SPARK Vec2>("a_TexCoord", Spark::AttributeType::VEC2);

	struct Vertex {
		_SPARK Vec3 position;
		_SPARK Vec3 normal;
		_SPARK Vec2 texcoord;
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
	material.SetParameter("u_Color", _SPARK Vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Set up instance data
	Spark::VertexLayout instance_layout;
	instance_layout.AddAttribute<Spark::Mat4>("a_Transform", Spark::AttributeType::MAT4);

	Spark::Vector<_SPARK Mat4> transforms;
	for (i32 i = 0; i < 5; ++i) {
		_SPARK Mat4 transform = Spark::Math::CreateTranslationMatrix(_SPARK Vec3(i * 1.0f - 2.0f, 0.0f, 0.0f));
		transforms.PushBack(transform);
	}

	mesh.SetInstanceData(transforms, instance_layout);

	app.CreateAsset<Spark::Camera>("main");
}

// Render system
void RenderSystem(Spark::Application& app) {
	auto&        camera   = app.GetAsset<Spark::Camera>("main");
	auto& material = app.GetAsset<Spark::GLMaterial>("triangle_material");
	auto& mesh     = app.GetAsset<Spark::GLGenericMesh>("triangle");
	auto& renderer = app.GetRenderer();

	material.SetParameter("u_ViewProjection", camera.GetViewMatrix() * camera.GetProjectionMatrix());

	auto cmd = renderer.CreateInstancedDrawCommand(mesh, material, 5);
	renderer.Submit(cmd);
}

int main() {
	Spark::Application app(Spark::GraphicsAPI::OPENGL);

	// Setup window and renderer
	app.CreateWindow<Spark::GLWindow>("Generic Renderer Demo", 1920 / 2, 1080 / 2).CreateRenderer<Spark::GLRenderer>();
	// Add scene setup system
	app.AddSystem(SetupScene, Spark::SystemInfo{.stage = Spark::SystemStage::Start});

	// Add render system
	app.AddSystem(RenderSystem, Spark::SystemInfo{.stage = Spark::SystemStage::Render});

	// Add resource
	app.AddResource(Spark::FPSResource{});

	// Add FPS system (without explicit template parameter)
	app.AddSystem([](Spark::ResMut<Spark::FPSResource> fps) { fps->calculator.Update(); },
	              Spark::SystemInfo{
					  .stage = Spark::SystemStage::PostRender,
				  });

	app.Start();
	return 0;
}