#include <core/pch.hpp>
#include <core/util/types.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/event/event.hpp>
#include <core/app/application.hpp>
#include <core/ecs/ecs.hpp>
#include <core/system/serializer.hpp>
#include <core/system/file_serializer.hpp>
#include <core/system/manager.hpp>
#include <core/ecs/components/transform_component.hpp>
#include <core/render/gl/gl_renderer.hpp>

void test_event_fn(Spark::Application& app, const std::shared_ptr<Spark::Event> event) {
	LOG_INFO("EVENT HAPPENED");
}

void test_query_event(Spark::Application& app, Spark::Query<Spark::TransformComponent>& query, const std::shared_ptr<Spark::Event> event) {
	for (auto e : query) {
		LOG_INFO(e->GetEntityId());
	}
};

void create_entities(Spark::Application& app) {
	auto & ecs = app.GetEcs();

	for (auto i = 0; i < 10; i++) {	
		auto e = ecs.MakeEntity();
		ecs.AddComponent(e, "transform", new Spark::TransformComponent(glm::vec3(i, i, i)));
	}
}

static f64 fps = 0.9;
static std::chrono::time_point<std::chrono::steady_clock> last = std::chrono::steady_clock::now();

void set_window_title_fps(Spark::Application& app) {
	auto& window = app.GetWindow();
	auto start = std::chrono::steady_clock::now();
	std::chrono::duration<f64> duration = start - last;

	fps = 1.0 / duration.count();

	std::string fps_avg = std::to_string(static_cast<i32>(std::round(fps)));
	window.SetTitle("Fps: " + fps_avg);
	last = start;
} 

i32 main()
{
	Spark::Application app(Spark::GraphicsAPI::OpenGL);

	app.CreateWindow<Spark::GLWindow>("Title", 1000, 1000)
		.CreateRenderer<Spark::GLRenderer>()
		.AddStartupFunction(create_entities)
		.AddUpdateFunction(set_window_title_fps)
		.AddEventFunction(EVENT_TYPE_KEY_HELD, test_event_fn)
		.AddQueryEventFunction<Spark::TransformComponent>(EVENT_TYPE_MOUSE_BUTTON_PRESSED, test_query_event)
		.Start();
}