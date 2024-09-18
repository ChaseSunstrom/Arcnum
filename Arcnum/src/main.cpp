#include <core/render/gl/gl_renderer.hpp>
#include <core/spark.hpp>
#include <core/window/gl/gl_window.hpp>

void MouseEvents(Spark::Application& app, const Spark::MultiEventPtr<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>& event) {
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

	std::vector<u32> indices = {0, 1, 2, 2, 1, 0};
	mesh_manager.Create<Spark::GLStaticMesh>("triangle", vertices, indices)->CreateMesh();
}

void CreateSimpleMaterial(Spark::Application& app) {
	auto& material_manager = app.GetManager<Spark::Material>();
	material_manager.Create<Spark::GLMaterial>("simple_material")->SetAlbedo(Spark::Math::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void CreateTriangleModel(Spark::Application& app) {
	auto& model_manager    = app.GetManager<Spark::StaticModel>();
	auto& mesh_manager     = app.GetManager<Spark::StaticMesh>();
	auto& material_manager = app.GetManager<Spark::Material>();

	auto triangle_mesh     = mesh_manager.Get("triangle");
	auto simple_material   = material_manager.Get("simple_material");

	model_manager.Create("triangle_model", triangle_mesh, simple_material);
}

void AddTriangleToScene(Spark::Application& app) {
	auto& scene_manager = app.GetManager<Spark::Scene>();
	auto  current_scene = scene_manager.GetCurrentScene();

	// Add multiple instances to ensure instanced rendering is triggered
	for (i32 i = 0; i < 10; ++i) {
		Spark::Math::Mat4 transform = Spark::Math::Translate(Spark::Math::Mat4(1.0f), Spark::Math::Vec3(i * 0.5f, 0.0f, 0.0f));
		current_scene->AddModelInstance(app.GetEcs().MakeEntity(), "triangle_model", transform);
	}
}

void SetupTestScene(Spark::Application& app) {
	CreateTriangleMesh(app);
	CreateSimpleMaterial(app);
	CreateTriangleModel(app);
	AddTriangleToScene(app);
}

void UpdateCamera(Spark::Application& app) {
	auto cam = app.GetManager<Spark::Camera>().GetCurrentCamera();
	cam->SetFar(100000.0f);
	//cam->MoveX(2.44f);
	//cam->Rotate(Spark::Math::Vec3(45.0f, 1.0f, 45.0f));
}

void TestVector(Spark::Application& app) {
	Spark::Vector vec = {1, 2, 3, 4, 5};

	vec += 6;
	vec += {7, 8, 9};
	vec += {10, 11, 12, 13, 14};

	vec -= 10;

	LOG_WARN(vec);

	std::vector<i32, Spark::Allocator<i32>> test_vec;

	test_vec.push_back(1);
	test_vec.push_back(2);

	for (auto& i : test_vec) {
		LOG_WARN(i);
	}
}

void TestString(Spark::Application& app) {
	Spark::String str  = "Hello, World! ";
	bool          test = true;
	f32           x    = 1.019981;
	str += test;
	str += " ";
	str += x;
	LOG_WARN(str);
}

void TestCallable(Spark::Application& app) {
	Spark::Callable<i32(i32, i32)>                   fn       = [](i32 x, i32 y) -> i32 { return x + y; };
	Spark::Callable<void(const Spark::Application&)> other_fn = [](const Spark::Application& app) {};
	LOG_WARN(fn(1, 2));
}

i32 main() {
	Spark::Application app(Spark::GraphicsAPI::OpenGL);
	app.CreateWindow<Spark::GLWindow>("FPS Counter", 1920 / 2, 1080 / 2)
		.CreateRenderer<Spark::GLRenderer>()
		.AddSystem<Spark::FPSSystem>()
		.AddStartupFunction(TestVector)
		.AddStartupFunction(TestString)
		.AddStartupFunction(TestCallable)
		.AddStartupFunction(SetupTestScene)
		.AddUpdateFunction(UpdateCamera)
		//.AddStartupFunction(startup_fn2, {true, false})
		.AddEventFunction<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>(MouseEvents, {true, false})
		.Start();
}