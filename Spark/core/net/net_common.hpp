#ifndef SPARK_NET_COMMON_HPP
#define SPARK_NET_COMMON_HPP

#include <boost/asio.hpp>

#include "packet.hpp"

#include "../events/event.hpp"



namespace spark
{
	namespace net
	{
		namespace asio = boost::asio;

		inline asio::io_context& default_io_context()
		{
			static asio::io_context io_context;
			return io_context;
		}

		struct udp_server_receive_event : public event
		{
			udp_server_receive_event() = default;

			udp_server_receive_event(std::unique_ptr<packet> packet) :
				event(UDP_SERVER_RECEIVE_EVENT), m_packet(std::move(packet)) {}
		
			std::unique_ptr<packet> m_packet;
		};

		struct udp_client_receive_event : public event
		{
			udp_client_receive_event() = default;

			udp_client_receive_event(std::unique_ptr<packet> packet) :
				event(UDP_CLIENT_RECEIVE_EVENT), m_packet(std::move(packet)) {}

			std::unique_ptr<packet> m_packet;
		};
		
		// Similar thing to UDP, need both for event organization
		struct tcp_client_receive_event : public event
		{
			tcp_client_receive_event() = default;

			tcp_client_receive_event(std::unique_ptr<packet> packet) :
				event(TCP_CLIENT_RECEIVE_EVENT), m_packet(std::move(packet)) {}

			std::unique_ptr<packet> m_packet;
		};

		struct tcp_server_receive_event : public event
		{
			tcp_server_receive_event() = default;

			tcp_server_receive_event(std::unique_ptr<packet> packet) :
				event(TCP_SERVER_RECEIVE_EVENT), m_packet(std::move(packet)) {}

			std::unique_ptr<packet> m_packet;
		};
	}
}

#endif