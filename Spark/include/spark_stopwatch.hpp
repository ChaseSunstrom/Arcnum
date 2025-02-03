
#ifndef SPARK_STOPWATCH_HPP
#define SPARK_STOPWATCH_HPP

#include "spark_pch.hpp"

namespace spark
{
    template <typename Ty = f64, typename Duration = std::chrono::duration<Ty>>
    class Stopwatch
    {
        static_assert(std::is_floating_point_v<Ty>, "Stopwatch only supports floating-point types.");

    public:
        Stopwatch()
            : m_start_time(), m_elapsed_time(Duration::zero()), m_is_running(false)
        {}

        void Start()
        {
            if (!m_is_running)
            {
                m_is_running = true;
                m_start_time = Clock::now();
            }
        }

        void Stop()
        {
            if (m_is_running)
            {
                auto now = Clock::now();
                m_elapsed_time += std::chrono::duration_cast<Duration>(now - m_start_time);
                m_is_running = false;
            }
        }

        void Reset()
        {
            m_elapsed_time = Duration::zero();
            m_is_running = false;
        }

        Ty GetElapsedTime() const
        {
            if (m_is_running)
            {
                auto now = Clock::now();
                auto current_elapsed = m_elapsed_time + std::chrono::duration_cast<Duration>(now - m_start_time);
                return current_elapsed.count();
            }
            return m_elapsed_time.count();
        }

        bool IsRunning() const { return m_is_running; }

    private:
        using Clock = std::chrono::steady_clock;

        Clock::time_point m_start_time;
        Duration m_elapsed_time;
        bool m_is_running;
    };

    template <typename Ty = f64>
    class FpsTracker
    {
    public:
        FpsTracker()
            : m_max_fps(std::numeric_limits<Ty>::lowest()),
            m_min_fps(std::numeric_limits<Ty>::max()),
            m_fps(0.0f),
            m_last_time(std::chrono::high_resolution_clock::now())
        {
        }

        void Update()
        {
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<Ty> delta_time =
                std::chrono::duration_cast<std::chrono::duration<Ty>>(current_time - m_last_time);

            m_last_time = current_time;

            //      cast to int for no weird floating point fps, and explicit cast to suppress warning
            m_fps = (f64)(u64)(1.0f / delta_time.count());

            if (m_fps > m_max_fps)
            {
                m_max_fps = m_fps;
            }
            if (m_fps < m_min_fps)
            {
                m_min_fps = m_fps;
            }
        }

		Ty GetMaxFps() const { return m_max_fps; }
		Ty GetMinFps() const { return m_min_fps; }
		Ty GetFps() const { return m_fps; }

        friend std::ostream& operator<<(std::ostream& os, const FpsTracker& ft)
        {
            os << "Max Fps: " << ft.m_max_fps << "\n";
            os << "Min Fps: " << ft.m_min_fps << "\n";
            os << "Fps: " << ft.m_fps;
            return os;
        }
    private:
        std::chrono::high_resolution_clock::time_point m_last_time;
        Ty m_max_fps;
        Ty m_min_fps;
        Ty m_fps;
    };
}

#endif
