#ifndef SPARK_RENDERER_H
#define SPARK_RENDERER_H

#include "../spark.hpp"

#include "../scene/scene.hpp"
#include "../events/event.hpp"
#include "../ecs/ecs.hpp"
#include "../logging/log.hpp"
#include "instancer.hpp"

namespace spark
{
	// ==============================================================================
	// RENDERER:     | Used for rendering and storing rendering data
	// ==============================================================================

	class renderer
	{
	public:
		renderer() = default;

		~renderer() = default;

		inline void on_update()
		{
			calculate_delta_time();
			calculate_last_frame_time();
			calculate_total_time();
		}

		void render(scene& scene);
		
		inline void calculate_total_time()
		{
			m_total_time += m_delta_time;
		}

		inline void calculate_delta_time()
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration <float64_t> elapsed = now - m_last_frame_time_point;
			m_delta_time = elapsed.count(); // Time between the last frame and the current frame in seconds
			m_last_frame_time_point = now;
		}

		inline void calculate_last_frame_time()
		{
			m_last_frame_time = m_delta_time * 1000.0; // Convert seconds to milliseconds
		}

		inline float64_t get_fixed_delta_time() { return m_fixed_delta_time; }

		inline float64_t get_delta_time() { return m_delta_time; }

		inline float64_t get_last_frame_time() { return m_last_frame_time; }

		inline float64_t get_total_time() { return m_total_time; }

		inline uint64_t get_tick_speed() { return m_tick_speed; }

		inline void set_fixed_delta_time(float64_t time) { m_fixed_delta_time = time; }

		inline void set_tick_speed(uint64_t speed) { m_tick_speed = speed; }

		inline instancer& get_instancer() { return *m_instancer; }
	private:
		float64_t m_fixed_delta_time = 0.005;

		float64_t m_delta_time = 0;

		float64_t m_last_frame_time = 0;

		float64_t m_total_time = 0;

		uint64_t m_tick_speed = 60;

		// Time when renderer was constructed (used for time calculations)
		std::chrono::time_point <std::chrono::high_resolution_clock> m_start_time = std::chrono::high_resolution_clock::now();

		std::chrono::time_point <std::chrono::high_resolution_clock> m_last_frame_time_point = std::chrono::high_resolution_clock::now();

		std::unique_ptr <instancer> m_instancer = std::make_unique<instancer>();
	};
}

#endif //CORE_RENDERER_H