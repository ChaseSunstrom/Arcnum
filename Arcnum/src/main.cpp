#include <core/render/gl/gl_renderer.hpp>
#include <core/spark.hpp>
#include <core/window/gl/gl_window.hpp>

void startup_fn(Spark::Application& app) {
	auto& sm = app.GetManager<Spark::Scene>();
	auto& s  = sm.Create("Scene1");
}

void startup_fn2(Spark::Application& app) {
	auto& ecs = app.GetEcs();

	for (i32 i = 0; i < 50; i++) {
		auto& e = ecs.MakeEntity(Spark::PairOf("transform", Spark::TransformComponent(glm::vec3(0))));
		// ecs.AddComponent(e, "model", Spark::ModelComponent("model"));
	}
}

void test_event(Spark::Application& app, const Spark::EventPtr<Spark::EntityCreatedEvent>& event) {
	auto& ecs = app.GetEcs();
	// ecs.DestroyEntity(event->entity.GetId());
}

void test_event2(Spark::Application& app, const Spark::MultiEventPtr<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>& event) {
	if (event->Is<Spark::MouseButtonPressedEvent>()) {
		auto pressed_event = event->Get<Spark::MouseButtonPressedEvent>();
		LOG_INFO("Mouse Pressed: " << pressed_event->button);
	} else if (event->Is<Spark::MouseButtonReleasedEvent>()) {
		auto released_event = event->Get<Spark::MouseButtonReleasedEvent>();
		LOG_INFO("Mouse Released: " << released_event->button);
	}
}

void CreateTriangleMesh(Spark::Application& app) {
	auto& mesh_manager                  = app.GetManager<Spark::StaticMesh>();

	std::vector<Spark::Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
		{ {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		{  {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
    };

	std::vector<u32> indices = {0, 1, 2};

	auto& triangle_mesh      = mesh_manager.Create<Spark::GLStaticMesh>("triangle", vertices, indices);
	triangle_mesh.CreateMesh();
}

void CreateSimpleMaterial(Spark::Application& app) {
	auto& material_manager = app.GetManager<Spark::Material>();
	auto& shader_manager   = app.GetManager<Spark::RenderShader>();
	auto& material         = material_manager.Create<Spark::GLMaterial>("simple_material");
	material.SetAlbedo(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
}

void CreateTriangleModel(Spark::Application& app) {
	auto& model_manager    = app.GetManager<Spark::StaticModel>();
	auto& mesh_manager     = app.GetManager<Spark::StaticMesh>();
	auto& material_manager = app.GetManager<Spark::Material>();

	auto& triangle_mesh    = mesh_manager.Get("triangle");
	auto& simple_material  = material_manager.Get("simple_material");

	auto& triangle_model   = model_manager.Create("triangle_model", triangle_mesh, &simple_material);
	triangle_model.SetModelMatrix(glm::mat4(1.0f)); // Identity matrix for now
}

void AddTriangleToScene(Spark::Application& app) {
	auto& scene_manager = app.GetManager<Spark::Scene>();
	auto& current_scene = scene_manager.GetCurrentScene();

	// Add multiple instances to ensure instanced rendering is triggered
	for (int i = 0; i < 100; ++i) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(i * 0.5f, 0.0f, 0.0f));
		current_scene.AddModelInstance(app.GetEcs().MakeEntity(), "triangle_model", transform);
	}
}

void SetupTestScene(Spark::Application& app) {
	CreateTriangleMesh(app);
	CreateSimpleMaterial(app);
	CreateTriangleModel(app);
	AddTriangleToScene(app);
}

void change_camera_pos(Spark::Application& app) {
	auto& cam = app.GetManager<Spark::Camera>().GetCurrentCamera();
	cam.SetPosition(cam.GetPosition() + Spark::Math::Vec3(0.001f, 0.0f, 0.0f));
}

i32 main() {
	Spark::Application app(Spark::GraphicsAPI::OpenGL);

	app.CreateWindow<Spark::GLWindow>("FPS Counter", 1920 / 2, 1080 / 2)
		.CreateRenderer<Spark::GLRenderer>()
		.AddSystem<Spark::FPSSystem>()
		.AddStartupFunction(startup_fn)
		.AddStartupFunction(SetupTestScene)
		.AddUpdateFunction(change_camera_pos)
		//.AddStartupFunction(startup_fn2, {true, false})
		.AddEventFunction<Spark::EntityCreatedEvent>(test_event)
		.AddEventFunction<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>(test_event2, {true, false})
		.Start();
}