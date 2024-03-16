#include "test.hpp"

#include "../util/memory.hpp"
#include "../net/udp.hpp"
#include "../net/tcp.hpp"

namespace net = spark::net;

class chat_message : public net::packet
{
public:
    chat_message() : packet("chat_message", 1) {};
    chat_message(const std::string& msg) : 
        packet("chat_message", 1), m_message(msg) {}

    void process() const override 
    {
        std::cout << "Received chat message: " << m_message << std::endl;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<packet>(*this);
        ar& m_message;
    }
public:
    std::string m_message;
private:
    friend class boost::serialization::access;
};

REGISTER_PACKET_TYPE(chat_message, 1);

namespace spark
{
	namespace test
	{
        struct test_resource 
        {
            int32_t value;
            explicit test_resource(int32_t val) : value(val) {}
        };

        TEST(test_unique_shield_ptr)
        {
            unique_shield_ptr<test_resource> ptr(new test_resource(10));
            EXPECT_EQ(ptr.access()->value, 10);
            ptr.reset(new test_resource(20));
            EXPECT_EQ(ptr.access()->value, 20);
        }

        TEST(test_shared_shield_ptr)
        {
            shared_shield_ptr<test_resource> ptr1(new test_resource(10));
            EXPECT_EQ(ptr1.access()->value, 10);
            {
                shared_shield_ptr<test_resource> ptr2 = ptr1;
                EXPECT_EQ(ptr1.access()->value, ptr2.access()->value);
                EXPECT_EQ(ptr2.use_count(), 2);
            }
            EXPECT_EQ(ptr1.use_count(), 1);
        }
        
        TEST(make_threaded_unique_ptr)
        {
            auto ptr = std::make_unique<test_resource>(30);
            auto ptr2 = make_threaded<test_resource>(std::move(ptr));
            EXPECT_EQ(ptr2.access()->value, 30);
        }
        
        TEST(make_threaded_shared_ptr)
        {
            auto ptr = std::make_shared<test_resource>(40);
            auto ptr2 = make_threaded<test_resource>(ptr);
            EXPECT_EQ(ptr2.access()->value, 40);
        }
        
        TEST(test_threaded_shared_ptr)
        {
            shared_shield_ptr<int32_t> ptr(new int32_t(0));
            std::thread t1([&ptr] { for (int32_t i = 0; i < 1000; ++i) ++(*ptr.access()); });
            std::thread t2([&ptr] { for (int32_t i = 0; i < 1000; ++i) ++(*ptr.access()); });
            t1.join();
            t2.join();
            EXPECT_EQ(*ptr.access(), 2000); // This should pass if the shared_shield_ptr properly locks around the int increment.
        }

        //TEST(test_udp_server)
        //{
        //    spark::thread_pool::enqueue(spark::task_priority::HIGH, []()
        //        {
        //            net::udp_server server("0.0.0.0", "12345");
        //        });

        //    spark::net::udp_client client("127.0.0.1", "8080");

        //    std::string line;
        //    while (std::getline(std::cin, line)) {
        //        chat_message msg(line);
        //        client.send(msg);
        //    }
        //}

        TEST(test_tcp_server)
        {
            spark::thread_pool::enqueue(spark::task_priority::HIGH, []()
                {
                    net::tcp_server server("127.0.0.1", "8080");
                });

            net::tcp_client client("127.0.0.1", "8080");

            std::string line;
            while (std::getline(std::cin, line)) 
            {
                chat_message msg(line);
                client.send(msg);
            }
        }

        bool core_test_main()
        {
            try
            {
                for (const auto& test : test_register::get_tests())
                {
                    test.function();

					SPARK_INFO("[CORE TEST PASSED]: " << test.name);
                }
            }
            catch (const std::exception& e)
            {
                SPARK_ERROR("[CORE TEST FAILED]: " << e.what());
                return false;
            }
            return true;
        }
	}
}