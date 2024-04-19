#include "spark.hpp"

#include "app/app.hpp"
#include "ecs/ecs.hpp"
#include "renderer/instancer.hpp"
#include "util/thread_pool.hpp"
#include "tests/test.hpp"

namespace spark
{
	// Default main function
	void spark_main()
	{
		u32 num_threads = std::thread::hardware_concurrency();

		if (num_threads == 0)
		{
			num_threads = 2;
		}

		thread_pool::initialize(num_threads);

#define SPARK_TEST
#ifndef SPARK_TEST
		instancer::get();
		vulkan_window::get();
		application::on_start();
#else	
		test::core_test_main();
#endif

		thread_pool::shutdown();
	}
}