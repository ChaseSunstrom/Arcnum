#ifndef SPARK_TEST_HPP
#define SPARK_TEST_HPP

#include "../spark.hpp"

#include "../logging/log.hpp"

namespace spark
{
	namespace test
	{
		class TestRegistry
		{
		public:

			struct Test
			{
				Test() = default;

				Test(const std::string& name, std::function<void()> function) :
						name(name), function(function) { }

				std::string name;

				std::function<void()> function;
			};

			static void add_test(const std::string& name, std::function<void()> function)
			{
				get_tests().emplace_back(Test(name, function));
			}

			static std::vector <Test>& get_tests()
			{
				static std::vector <Test> tests;
				return tests;
			}
		};

#define CONCAT_INTERNAL(x, y) x##y
#define CONCAT(x, y) CONCAT_INTERNAL(x,y)

#define EXPECT_EQ(a, b) do { \
    if ((a) != (b)) { \
        SPARK_ERROR("[TEST FAILED]: Expected " << (a) << " == " << (b)); \
        throw std::runtime_error("Test failed."); \
    } \
} while(0)

#define EXPECT_NE(a, b) do { \
    if ((a) == (b)) { \
        SPARK_ERROR("[TEST FAILED]: Expected " << (a) << " != " << (b)); \
        throw std::runtime_error("Test failed."); \
    } \
} while(0)

#define EXPECT_TRUE(a) do { \
	if (!(a)) { \
		SPARK_ERROR("[TEST FAILED]: Expected " << (a) << " == true"); \
		throw std::runtime_error("Test failed."); \
	} \
} while(0)

#define EXPECT_FALSE(a) do { \
	if ((a)) { \
		SPARK_ERROR("[TEST FAILED]: Expected " << (a) << " == false"); \
		throw std::runtime_error("Test failed."); \
	} \
} while(0)

#define TEST(name) \
             void name(); \
             namespace { \
                struct CONCAT(TestRegistry, name) { \
                    CONCAT(TestRegistry, name)() { \
                        TestRegistry::add_test(#name, name); \
                    } \
                } CONCAT(TestRegistryInstance, name); \
             } \
             void name()

		bool core_test_main();
	}
}

#endif // CORE_TEST_H