#ifndef SPARK_UDP_HPP
#define SPARK_UDP_HPP

#include <boost/asio.hpp>

#include "../spark.hpp"
#include "../util/thread_pool.hpp"
#include "../logging/log.hpp"

#include "packet.hpp"
#include "net_common.hpp"

namespace spark
{
	namespace net
	{
		using boost::asio::ip::udp;
		namespace asio = boost::asio;

       
        class udp_client 
        {
        public:
            udp_client(const std::string& host = "127.0.0.1", const std::string& port = "8080", asio::io_context& io_context = default_io_context())
                : m_socket(io_context), m_resolver(io_context)
            {
                udp::resolver::query query(udp::v4(), host, port);
                m_endpoint = *m_resolver.resolve(query).begin();
                m_socket.open(udp::v4());
            }

            template<typename T>
            void send(const T& packet)
            {
                std::string serialized_packet = serialize(packet);
                m_socket.send_to(asio::buffer(serialized_packet), m_endpoint);
            }
        private:
            udp::socket m_socket;
            udp::resolver m_resolver;
            udp::endpoint m_endpoint;
        };

        class udp_server
        {
        public:
            udp_server(const std::string& ip = "127.0.0.1", const std::string& port = "8080", asio::io_context& io_context = default_io_context())
                : m_socket(io_context), m_work(io_context)
            {
                udp::endpoint local_endpoint(asio::ip::make_address(ip), static_cast<uint16_t>(std::stoi(port)));

                m_socket.open(udp::v4());
                m_socket.bind(local_endpoint);

                SPARK_INFO("[UDP SERVER STARTED]: [IP]: " << ip << " [PORT]: " << port);

                start_receive(); // Start receiving packets asynchronously

                io_context.run();

				SPARK_INFO("[UDP SERVER STOPPED]: [IP]: " << ip << " [PORT]: " << port);
            }

            ~udp_server()
            {
                if (m_socket.is_open())
				    m_socket.close();
            }

            void close()
            {
				if (m_socket.is_open())
				    m_socket.close();
            }

        private:
            void start_receive()
            {
                m_socket.async_receive_from(
                    asio::buffer(m_receive_buffer), m_remote_endpoint,
                    [this](boost::system::error_code ec, std::uint64_t bytes_recvd)
                    {
                        if (!ec && bytes_recvd > 0)
                        {
                            try
                            {
                                auto [type, version, data] = deserialize(std::string(m_receive_buffer.begin(), m_receive_buffer.begin() + bytes_recvd));
                                auto packet = packet_factory_registry::create_packet(type, version, data);
                                SPARK_TRACE("[UDP SERVER RECEIVED PACKET]: [TYPE]:" << type << " [VERSION]:" << std::to_string(version));
                                if (packet)
                                {
                                    packet->process();
                                }
                                else
                                {
                                    SPARK_FATAL("[UDP SERVER UNABLE TO PROCESS PACKET]: [TYPE]:" << type << " [VERSION]:" << std::to_string(version));
                                }
                            }
                            catch (const std::exception& e)
                            {
                                std::cerr << "Error processing packet: " << e.what() << std::endl;
                            }
                            start_receive(); // Continue listening for incoming packets
                        }
                    });
            }

            udp::socket m_socket;
            udp::endpoint m_remote_endpoint;
            std::array<uint8_t, 4096> m_receive_buffer;
            asio::io_context::work m_work; // Ensure the io_context doesn't exit prematurely
        };
	}
}

#endif