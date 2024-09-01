#include <array>
#include <chrono>
#include <core/app/application.hpp>
#include <core/render/gl/gl_renderer.hpp>
#include <core/render/renderer.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/window/window.hpp>
#include <core/ecs/components/transform_component.hpp>

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
	auto& s = sm.Create("Scene1");
}

void startup_fn2(Spark::Application& app) {
	auto& ecs = app.GetEcs();

	for (i32 i = 0; i < 5000; i++)
	{
		ecs.MakeEntity(std::make_pair("transform", new Spark::TransformComponent(glm::vec3(i))));
	}
}

i32 main() {
	Spark::Application app(Spark::GraphicsAPI::OpenGL);
	app.CreateWindow<Spark::GLWindow>("FPS Counter", 1000, 1000)
		.CreateRenderer<Spark::GLRenderer>()
		.AddStartupFunction(startup_fn)
		.AddStartupFunction(startup_fn2)
		.AddUpdateFunction(updateFPS, {true, false})
		.Start();
}