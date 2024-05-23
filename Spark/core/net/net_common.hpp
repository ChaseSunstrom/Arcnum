#ifndef SPARK_NET_COMMON_HPP
#define SPARK_NET_COMMON_HPP

#include "serializeable.hpp"

#include "../events/event.hpp"

namespace Spark
{
namespace net
{
namespace asio = boost::asio;

inline asio::io_context &default_io_context()
{
    static asio::io_context io_context;
    return io_context;
}

struct UDPServerReceiveEvent : public Event
{
    UDPServerReceiveEvent() = default;

    UDPServerReceiveEvent(std::unique_ptr<Serializeable> packet)
        : Event(UDP_SERVER_RECEIVE_EVENT), m_packet(std::move(packet))
    {
    }

    std::unique_ptr<Serializeable> m_packet;
};

struct UDPClientReceiveEvent : public Event
{
    UDPClientReceiveEvent() = default;

    UDPClientReceiveEvent(std::unique_ptr<Serializeable> packet)
        : Event(UDP_CLIENT_RECEIVE_EVENT), m_packet(std::move(packet))
    {
    }

    std::unique_ptr<Serializeable> m_packet;
};

// Similar thing to UDP, need both for event organization
struct TCPClientReceiveEvent : public Event
{
    TCPClientReceiveEvent() = default;

    TCPClientReceiveEvent(std::unique_ptr<Serializeable> packet)
        : Event(TCP_CLIENT_RECEIVE_EVENT), m_packet(std::move(packet))
    {
    }

    std::unique_ptr<Serializeable> m_packet;
};

struct TCPServerReceiveEvent : public Event
{
    TCPServerReceiveEvent() = default;

    TCPServerReceiveEvent(std::unique_ptr<Serializeable> packet)
        : Event(TCP_SERVER_RECEIVE_EVENT), m_packet(std::move(packet))
    {
    }

    std::unique_ptr<Serializeable> m_packet;
};
} // namespace net
} // namespace Spark

#endif