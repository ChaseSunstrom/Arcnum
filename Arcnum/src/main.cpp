#include "core/ecs/components/model_component.hpp"

#include <array>
#include <chrono>
#include <core/app/application.hpp>
#include <core/ecs/components/model_component.hpp>
#include <core/ecs/components/transform_component.hpp>
#include <core/render/gl/gl_renderer.hpp>
#include <core/render/renderer.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/window/window.hpp>

class FPSCalculator {
  private:
	static constexpr size_t MAX_SAMPLES                    = 120; // 2 seconds at 60 FPS
	static constexpr std::chrono::duration UPDATE_INTERVAL = std::chrono::milliseconds(500);

	std::array<double, MAX_SAMPLES> frameTimes;
	size_t frameIndex = 0;
	size_t frameCount = 0;
	std::chrono::steady_clock::time_point lastUpdateTime;
	std::chrono::steady_clock::time_point lastFrameTime;
	double currentFPS = 0.0;

  public:
	FPSCalculator()
		: lastUpdateTime(std::chrono::steady_clock::now())
		, lastFrameTime(std::chrono::steady_clock::now()) {}

	void update(Spark::Application& app) {
		auto currentTime                        = std::chrono::steady_clock::now();
		std::chrono::duration<double> frameTime = currentTime - lastFrameTime;
		lastFrameTime                           = currentTime;

		frameTimes[frameIndex]                  = frameTime.count();
		frameIndex                              = (frameIndex + 1) % MAX_SAMPLES;
		if (frameCount < MAX_SAMPLES)
			frameCount++;

		if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
			double totalTime = 0.0;
			for (size_t i = 0; i < frameCount; ++i) {
				totalTime += frameTimes[i];
			}
			double averageFrameTime = totalTime / frameCount;
			currentFPS              = 1.0 / averageFrameTime;

			std::string title       = "FPS: " + std::to_string(static_cast<int>(currentFPS));
			std::cout << title << "\n";

			lastUpdateTime = currentTime;
		}
	}

	double getFPS() const { return currentFPS; }
};

FPSCalculator fpsCalculator;

void updateFPS(Spark::Application& app) { fpsCalculator.update(app); }

void startup_fn(Spark::Application& app) {
	auto& sm = app.GetManager<Spark::Scene>();
	auto& s  = sm.Create("Scene1");
}

void startup_fn2(Spark::Application& app) {
	auto& ecs = app.GetEcs();

	for (i32 i = 0; i < 5000; i++) {
		auto& e = ecs.MakeEntity(std::make_pair("transform", new Spark::TransformComponent(glm::vec3(0))));
		ecs.AddComponent(e, "model", new Spark::ModelComponent("model"));
	}
}

void test_event(Spark::Application& app, const std::shared_ptr<Spark::ComponentAddedEvent<Spark::TransformComponent>> event) {
	LOG_TRACE("Component Added: " << event->entity.GetId());
}

void test_event2(Spark::Application& app, const std::shared_ptr<Spark::BaseEvent> event) {
	if (auto mouse_pressed = std::dynamic_pointer_cast<Spark::MouseButtonPressedEvent>(event)) {
		LOG_INFO("Mouse Pressed: " << mouse_pressed->button);
	} else if (auto mouse_released = std::dynamic_pointer_cast<Spark::MouseButtonReleasedEvent>(event)) {
		LOG_INFO("Mouse Released: " << mouse_released->button);
	}
}

i32 main() {
	Spark::Application app(Spark::GraphicsAPI::OpenGL);
	app.CreateWindow<Spark::GLWindow>("FPS Counter", 1000, 1000)
		.CreateRenderer<Spark::GLRenderer>()
		.AddStartupFunction(startup_fn)
		.AddStartupFunction(startup_fn2)
		.AddUpdateFunction(updateFPS, {true, false})
		.AddSingleEventFunction<Spark::ComponentAddedEvent<Spark::TransformComponent>>(test_event)
		.AddEventFunction<Spark::MouseButtonPressedEvent, Spark::MouseButtonReleasedEvent>(test_event2)
		.Start();
}