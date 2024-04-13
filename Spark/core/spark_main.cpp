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

//#define TEST_MAIN
#ifdef TEST_MAIN
		test::core_test_main();
#else
		// To make sure the instancer is initialized
		instancer::get();

		application::on_start();
#endif
	}
}