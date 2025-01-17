
#ifndef SPARK_STOPWATCH_HPP
#define SPARK_STOPWATCH_HPP

#include "spark_pch.hpp"

namespace spark
{
    template <typename Ty = f64, typename Duration = std::chrono::milliseconds>
    class Stopwatch
    {
        static_assert(std::is_floating_point_v<Ty>, "Timer only supports floating-point types.");

    public:
        Stopwatch()
            : m_start_time(Ty(0))
            , m_end_time(Ty(0))
            , m_is_running(false)
        {}

        void Start()
        {
            if (!m_is_running)
            {
                m_is_running = true;
                m_start_time = GetElapsedDuration();
            }
        }

        void Stop()
        {
            if (m_is_running)
            {
                m_end_time = GetElapsedDuration();
                m_is_running = false;
            }
        }

        void Reset()
        {
            m_start_time = Ty(0);
            m_end_time = Ty(0);
            m_is_running = false;
        }

        Ty GetElapsedTime() const
        {
            if (m_is_running)
            {
                return GetElapsedDuration() - m_start_time;
            }
            return m_end_time > m_start_time ? m_end_time - m_start_time : Ty(0);
        }

        bool IsRunning() const { return m_is_running; }

    private:
        static Ty GetElapsedDuration()
        {
            auto current = std::chrono::high_resolution_clock::now().time_since_epoch();
            auto duration_count = std::chrono::duration_cast<Duration>(current).count();
            return static_cast<Ty>(duration_count);
        }

    private:
        Ty m_start_time;
        Ty m_end_time;
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
