#include "spark.hpp"

#include "app/app.hpp"
#include "ecs/ecs.hpp"
#include "renderer/instancer.hpp"
#include "tests/test.hpp"
#include "window/window_manager.hpp"
#include "util/thread_pool.hpp"

namespace Spark
{
// Default main function
void spark_main()
{
    u32 num_threads = std::thread::hardware_concurrency();

    if (num_threads == 0)
    {
        num_threads = 2;
    }

    ThreadPool::initialize(num_threads);

// #define SPARK_TEST
#ifndef SPARK_TEST
    Application::on_start();
#else
    test::core_test_main();
#endif

    ThreadPool::shutdown();
}
} // namespace Spark