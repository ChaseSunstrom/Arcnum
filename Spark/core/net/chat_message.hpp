#ifndef SPARK_CHAT_MESSAGE_HPP
#define SPARK_CHAT_MESSAGE_HPP

#include "serializeable.hpp"

namespace spark
{
	namespace net
	{
		class chat_message :
				public packet
		{
		public:
			chat_message() :
					packet("chat_message", 1) { };

			chat_message(const std::string& msg) :
					packet("chat_message", 1), m_message(msg) { }

			void process() const override
			{
				SPARK_INFO("[CHAT]: " << m_message);
			}

		public:
			std::string m_message;

			SERIALIZE_MEMBERS(chat_message, m_message)

		private:
			friend class ::boost::serialization::access;
		};
	}
}

#ifndef CHAT_MESSAGE_PACKET
#define CHAT_MESSAGE_PACKET

REGISTER_PACKET_TYPE(spark::net, chat_message, 1);
#endif // CHAT_MESSAGE_PACKET

#endif // SPARK_CHAT_MESSAGE_HPP