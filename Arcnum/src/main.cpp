#include <core/pch.hpp>
#include <core/util/types.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/event/input_events.hpp>
#include <core/event/event.hpp>
#include <core/app/application.hpp>
#include <core/ecs/ecs.hpp>
#include <core/system/serializer.hpp>
#include <core/system/file_serializer.hpp>
#include <core/system/manager.hpp>
#include <core/ecs/components/transform_component.hpp>
#include <core/render/gl/gl_renderer.hpp>
#include <core/util/thread_pool.hpp>


void test_event_fn(Spark::Application& app, const std::shared_ptr<Spark::Event> event) {
	LOG_INFO("EVENT HAPPENED");
}

void start_fn(Spark::Application& app) {
	auto& ecs = app.GetEcs();

	for (i32 i = 0; i < 50; i++)
	{
		auto e = ecs.MakeEntity(std::make_pair("transform", new Spark::TransformComponent(glm::vec3(0))));
	}
}

void remove_start_fn(Spark::Application& app) {
	auto& ecs = app.GetEcs();

	for (i32 i = 0; i < 50; i++)
	{
		ecs.DestroyEntity(i);
	}
}

void remove_component_fn(Spark::Application& app, const std::shared_ptr<Spark::Event> event) {
	LOG_TRACE("Component Removed: " << std::static_pointer_cast<Spark::ComponentRemovedEvent>(event)->GetComponent<Spark::TransformComponent>().GetEntityId());
}

void test_event(Spark::Application& app, const std::shared_ptr<Spark::Event> event) {
	LOG_TRACE("Component Added: " << std::static_pointer_cast<Spark::ComponentAddedEvent>(event)->GetComponent<Spark::TransformComponent>().GetEntityId());
};

void test_query_event(Spark::Application& app, Spark::Query<Spark::TransformComponent>& query, const std::shared_ptr<Spark::Event> event) {
	LOG_ERROR(std::static_pointer_cast<Spark::MouseButtonPressedEvent>(event)->button);
};

void create_entities(Spark::Application& app) {
	auto & ecs = app.GetEcs();

	for (auto i = 0; i < 10; i++) {	
		auto e = ecs.MakeEntity();
		ecs.AddComponent(e, "transform", new Spark::TransformComponent(glm::vec3(i, i, i)));
	}
}

static f64 fps = 0.;
static std::chrono::time_point<std::chrono::steady_clock> last = std::chrono::steady_clock::now();

void set_window_title_fps(Spark::Application& app) {
	auto& window = app.GetWindow();
	auto start = std::chrono::steady_clock::now();
	std::chrono::duration<f64> duration = start - last;

	std::string fps_avg = std::to_string(duration.count());
	window.SetTitle("Frame Time: " + fps_avg);
	last = start;
}

void print(Spark::Application& app) {
	auto& tp = app.GetThreadPool();
	tp.Enqueue(Spark::TaskPriority::LOW, false, [&tp]() {
		//tp.SyncThisThread(true);
			static std::atomic<i32> i(0);
			i32 local_i = i.fetch_add(1, std::memory_order_relaxed);
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			LOG_WARN(local_i);
		});
}

i32 main()
{
	Spark::Application app(Spark::GraphicsAPI::OpenGL);
	
	app .CreateWindow<Spark::GLWindow>("Title", 1000, 1000)
		.CreateRenderer<Spark::GLRenderer>()
		.AddStartupFunction(create_entities)
		.AddStartupFunction(start_fn)
		.AddStartupFunction(remove_start_fn)
		.AddUpdateFunction(set_window_title_fps, { false, false })
		.AddUpdateFunction(print, {true, false})
		.AddUpdateFunction(start_fn, {true, false})
		.AddUpdateFunction(remove_start_fn, {true, false})
		.AddEventFunction(EVENT_TYPE_KEY_HELD, test_event_fn, {true, true})
		.AddEventFunction(EVENT_TYPE_COMPONENT_ADDED, test_event, {false, false})
		.AddEventFunction(EVENT_TYPE_COMPONENT_REMOVED, remove_component_fn, {false, false})
		.AddQueryEventFunction<Spark::TransformComponent>(EVENT_TYPE_MOUSE_BUTTON_PRESSED, test_query_event)
		.Start();
}