#ifndef SPARK_FPS_HPP
#define SPARK_FPS_HPP

#include <core/ecs/system.hpp>
#include <core/pch.hpp>
#include <core/util/time.hpp>

namespace Spark {
	class FPSCalculator {
	  public:
		FPSCalculator()
			: m_last_update_time(std::chrono::steady_clock::now())
			, m_last_frame_time(std::chrono::steady_clock::now()) {}

		void Update() {
			auto                       current_time = std::chrono::steady_clock::now();
			std::chrono::duration<f64> frame_time   = current_time - m_last_frame_time;
			m_last_frame_time                       = current_time;
			m_frame_times[m_frame_index]            = frame_time.count();
			m_frame_index                           = (m_frame_index + 1) % MAX_SAMPLES;

			if (m_frame_count < MAX_SAMPLES)
				m_frame_count++;

			if (current_time - m_last_update_time >= UPDATE_INTERVAL) {
				f64 total_time = 0.0;
				for (size_t i = 0; i < m_frame_count; ++i) {
					total_time += m_frame_times[i];
				}
				f64 average_frame_time = total_time / m_frame_count;
				m_current_fps          = 1.0 / average_frame_time;
				std::string title      = std::to_string(static_cast<i32>(m_current_fps));
				LOG_INFO("FPS: " << title << "   " << std::setprecision(2) << average_frame_time << "ms");
				m_last_update_time = current_time;
			}
		}

		f64 GetFPS() const { return m_current_fps; }

	  private:
		static constexpr size_t                MAX_SAMPLES     = 120; // 2 seconds at 60 FPS
		static constexpr std::chrono::duration UPDATE_INTERVAL = std::chrono::milliseconds(500);

		std::array<f64, MAX_SAMPLES>          m_frame_times;
		size_t                                m_frame_index = 0;
		size_t                                m_frame_count = 0;
		std::chrono::steady_clock::time_point m_last_update_time;
		std::chrono::steady_clock::time_point m_last_frame_time;
		f64                                   m_current_fps = 0.0;
	};

	// Resource for FPS data
	struct FPSResource {
		FPSCalculator calculator;
	};

	// System parameter trait for delta time
	template<> struct SystemParamTrait<f32> {
		using Type = f32;
		static Type Extract(Application& app) { return app.GetDeltaTime(); }
	};

	// Create the FPS system as a function
	inline auto CreateFPSSystem() {
		return [fps = FPSCalculator()](f32) mutable { fps.Update(); };
	}

	// Alternative version using resource
	inline auto CreateFPSSystemWithResource() {
		return [](Application& app, ResMut<FPSResource> fps) { fps.Get().calculator.Update(); };
	}
} // namespace Spark


#endif