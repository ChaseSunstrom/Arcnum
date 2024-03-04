#ifndef SPARK_UDP_HPP
#define SPARK_UDP_HPP

#include <boost/asio.hpp>

#include "../spark.hpp"
#include "../util/thread_pool.hpp"

#include "packet.hpp"

namespace spark
{
	namespace net
	{
        using boost::asio::ip::udp;
        namespace asio = boost::asio;

        class udp_client
        {
        public:
   udp_client(asio::io_context& io_context, const std::string& host, const std::string& port)
                : m_socket(io_context), m_resolver(io_context)
            {
                udp::resolver::query query(udp::v4(), host, port);
                m_endpoint = *m_resolver.resolve(query).begin();
                m_socket.open(udp::v4()); // You only need to open the socket here
            }

            void send(const packet& packet) 
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
            udp_server(asio::io_context& io_context, const std::string& ip = "0.0.0.0", const std::string& port = "12345")
                : m_socket(io_context)
            {
                uint16_t port_num = static_cast<uint16_t>(std::stoi(port));
                m_remote_endpoint = udp::endpoint(boost::asio::ip::address::from_string(ip), port_num);

                m_socket.open(udp::v4());
                m_socket.bind(m_remote_endpoint);

                start_receive();
            }

        private:
            void start_receive() 
            {
                m_socket.async_receive_from(
                    asio::buffer(m_recieve_buffer), m_remote_endpoint,
                    [this](boost::system::error_code ec, std::size_t bytes_recvd) 
                    {
                        if (!ec && bytes_recvd > 0) 
                        {
                            std::cout << "Received " << bytes_recvd << " bytes" << std::endl;

                            thread_pool::enqueue(task_priority::HIGH, [this, bytes_recvd]() 
                                {
                                    std::string recv_data(m_recieve_buffer.begin(), m_recieve_buffer.begin() + bytes_recvd);
                                    auto packet = deserialize(recv_data);
                                    
                                    if (packet)
                                        packet->process();
                                });
                            start_receive();
                        }
                    });
            }
        private:
            udp::socket m_socket;
            udp::endpoint m_remote_endpoint;
            std::vector<uint8_t> m_recieve_buffer;
        };
	}
}

#endif