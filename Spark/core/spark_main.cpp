#include "spark.hpp"

#include "app/app.hpp"
#include "app/service.hpp"
#include "ecs/ecs.hpp"
#include "util/thread_pool.hpp"
#include "tests/test.hpp"

namespace spark
{
	// Default main function
	void spark_main()
	{
//#define TEST_MAIN
#ifdef TEST_MAIN
		test::core_test_main();
#else
		uint32_t num_threads = std::thread::hardware_concurrency();

		if (num_threads == 0)
		{
			num_threads = 1;
		}

		thread_pool::initialize(num_threads);

		application::on_start();
#endif
	}
}