#ifndef SPARK_SYSTEM_HPP
#define SPARK_SYSTEM_HPP

#include "../../spark.hpp"

#include "../../util/Singleton.hpp"
#include "../../util/memory.hpp"
#include "../../util/thread_pool.hpp"
#include "../component/component.hpp"

#include "../../net/serializeable.hpp"

namespace Spark
{
class System
{
  public:
    System() = default;
    virtual ~System() = default;

    virtual void on_init()
    {
    }

    virtual void on_start()
    {
    }

    virtual void on_update(f64 delta_time)
    {
    }

    virtual void on_shutdown()
    {
    }
    SERIALIZE_EMPTY()
};

class SystemManager : public Singleton<SystemManager>
{
    friend class Singleton<SystemManager>;
  public:

    template <typename T, typename... Args> T &register_update_system(Args &&...args)
    {
        std::unique_ptr<T> _system = std::make_unique<T>(std::forward<Args>(args)...);
        T &s = *_system;
        m_update_systems.push_back(std::static_pointer_cast<System>(std::move(_system)));
        return s;
    }

    template <typename T, typename... Args> T &register_start_system(Args &&...args)
    {
        std::unique_ptr<T> system = std::make_unique<T>(std::forward<Args>(args)...);
        T &s = *system;
        m_start_systems.push_back(std::move(system));
        return s;
    }

    template <typename T, typename... Args> T &register_shutdown_system(Args &&...args)
    {
        std::unique_ptr<T> system = std::make_unique<T>(std::forward<Args>(args)...);
        T &s = *system;
        m_shutdown_systems.push_back(std::move(system));
        return s;
    }

    void start_systems()
    {
        for (auto &system : m_start_systems)
        {
            system->on_start();
        }
    }

    void update_systems(f64 delta_time)
    {
        for (auto &system : m_update_systems)
        {
            ThreadPool::enqueue(TaskPriority::VERY_HIGH, true,
                                [system = system.get(), delta_time] { system->on_update(delta_time); });
        }
    }

    void shutdown_systems()
    {
        for (auto &system : m_shutdown_systems)
        {
            system->on_shutdown();
        }
    }

  private:
    SystemManager() = default;

    ~SystemManager()
    {
        shutdown_systems();
    }

  private:
    std::vector<std::unique_ptr<System>> m_start_systems = std::vector<std::unique_ptr<System>>();

    std::vector<std::unique_ptr<System>> m_update_systems = std::vector<std::unique_ptr<System>>();

    std::vector<std::unique_ptr<System>> m_shutdown_systems = std::vector<std::unique_ptr<System>>();

    SERIALIZE_MEMBERS(SystemManager, m_start_systems, m_update_systems, m_shutdown_systems)
};
} // namespace Spark

#endif