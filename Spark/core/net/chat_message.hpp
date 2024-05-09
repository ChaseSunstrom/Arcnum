#ifndef SPARK_CHAT_MESSAGE_HPP
#define SPARK_CHAT_MESSAGE_HPP

#include "serializeable.hpp"

namespace Spark
{
namespace net
{
class ChatMessage : public Packet
{
  public:
    ChatMessage() : Packet("chat_message", 1) {};

    ChatMessage(const std::string &msg) : Packet("chat_message", 1), m_message(msg)
    {
    }

    void process() const override
    {
        SPARK_INFO("[CHAT]: " << m_message);
    }

  public:
    std::string m_message;

    SERIALIZE_MEMBERS(ChatMessage, m_message)

  private:
    friend class ::boost::serialization::access;
};
} // namespace net
} // namespace Spark

#ifndef CHAT_MESSAGE_PACKET
#define CHAT_MESSAGE_PACKET

REGISTER_PACKET_TYPE(Spark::net, ChatMessage, 1);

#endif // CHAT_MESSAGE_PACKET

#endif // SPARK_CHAT_MESSAGE_HPP