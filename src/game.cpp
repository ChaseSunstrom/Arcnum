#include "core_include.hpp"
#include "game.hpp"

namespace math = spark::math;
namespace net = spark::net;

class chat_message : public spark::net::packet
{
public:
	std::string message;

	chat_message() = default;
	explicit chat_message(const std::string& msg) : message(msg) {}

	void process() const override {
		std::cout << "Received chat message: " << message << std::endl;
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& boost::serialization::base_object<packet>(*this);
		ar& message;
	}
};

BOOST_CLASS_EXPORT_GUID(chat_message, "chat_message")

void on_start()
{
	spark::thread_pool::enqueue(spark::task_priority::HIGH, []()
		{
			net::asio::io_context io_context;
			net::asio::io_context::work work(io_context); // Keeps io_context.run() from returning

			net::udp_server server(io_context, "127.0.0.1", "8080");
			io_context.run();
		});
}

void on_update()
{
	net::asio::io_context io_context;
	spark::net::udp_client client(io_context, "127.0.0.1", "8080");

	std::string line;
	while (std::getline(std::cin, line)) {
		chat_message msg(line);
		client.send(msg);
	}
}
// required function
// will recieve events from everything, it is automatically subscribed to recieve
// every event
bool on_event(std::shared_ptr<spark::event> event)
{
	return true;
}

// Registers functions to be called at different points in the application
// automatically
void register_functions()
{
	spark::app_functions::register_functions(on_start, on_update, on_event);
}