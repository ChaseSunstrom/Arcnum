#include "serializeable.hpp"

namespace spark
{
	namespace net
	{
		std::unordered_map <std::string, std::unique_ptr<IPacketFactory>> PacketFactoryRegistry::s_factories =
				std::unordered_map < std::string, std::unique_ptr<IPacketFactory>>

		();
	}
}