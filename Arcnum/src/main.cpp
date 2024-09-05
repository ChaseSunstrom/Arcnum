#include <core/spark.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/render/gl/gl_renderer.hpp>

void startup_fn(Spark::Application& app) {
	auto& sm = app.GetManager<Spark::Scene>();
	auto& s  = sm.Create("Scene1");
}

void startup_fn2(Spark::Application& app) {
	auto& ecs = app.GetEcs();

	for (i32 i = 0; i < 5000; i++) {
		auto& e = ecs.MakeEntity(Spark::PairOf("transform", Spark::TransformComponent(glm::vec3(0))));
		ecs.AddComponent(e, "model", Spark::ModelComponent("model"));
	}
}

void test_event(Spark::Application& app, const Spark::EventPtr<Spark::ComponentAddedEvent<Spark::TransformComponent>>& event) {
	LOG_TRACE("Component Added: " << event->entity.GetId());
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

i32 main() {
	Spark::Application app(Spark::GraphicsAPI::OpenGL);

	app.CreateWindow<Spark::GLWindow>("FPS Counter", 1000, 1000)
		.CreateRenderer<Spark::GLRenderer>()
		.AddSystem<Spark::FPSSystem>()
		//.AddStartupFunction(startup_fn)
		//.AddStartupFunction(startup_fn2)
		//.AddEventFunction<Spark::ComponentAddedEvent<Spark::TransformComponent>>(test_event)
		.AddEventFunction<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>(test_event2, {true, false})
		.Start();
}