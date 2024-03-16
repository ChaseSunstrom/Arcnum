#include "packet.hpp"

namespace spark
{
	namespace net
	{
		std::unordered_map<std::string, std::unique_ptr<base_packet_factory>> packet_factory_registry::s_factories = std::unordered_map<std::string, std::unique_ptr<base_packet_factory>>();
	}
}