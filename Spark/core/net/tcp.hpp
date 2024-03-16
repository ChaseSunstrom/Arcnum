#ifndef SPARK_TCP_HPP
#define SPARK_TCP_HPP

#include <boost/asio.hpp>
#include "../spark.hpp"
#include "packet.hpp"
#include "net_common.hpp"

namespace spark
{
    namespace net
    {
        using boost::asio::ip::tcp;
        namespace asio = boost::asio;

        class tcp_client
        {
        public:
            tcp_client(const std::string& host = "127.0.0.1", const std::string& port = "8080", asio::io_context& io_context = default_io_context())
                : m_io_context(io_context), m_socket(io_context)
            {
                tcp::resolver resolver(io_context);
                auto endpoints = resolver.resolve(host, port);
                boost::asio::connect(m_socket, endpoints);
            }

            template<typename T>
            void send(const T& packet)
            {
                std::string serialized_packet = serialize(packet);
                SPARK_INFO("[TCP CLIENT SENT PACKET]: " << serialized_packet);
                boost::asio::write(m_socket, asio::buffer(serialized_packet));
            }

            ~tcp_client()
            {
                m_socket.close();
            }

        private:
            asio::io_context& m_io_context;
            tcp::socket m_socket;
        };

        class tcp_server
        {
        public:
            tcp_server(const std::string& ip = "0.0.0.0", const std::string& port = "12345", asio::io_context& io_context = default_io_context()) :
                m_io_context(io_context),
                m_acceptor(io_context, tcp::endpoint(asio::ip::make_address(ip), static_cast<uint16_t>(std::stoi(port)))),
                m_socket(io_context)
            {
                m_acceptor.listen();

                accept_connection();

                SPARK_INFO("[TCP SERVER STARTED]: [IP]: " << ip << " [PORT]: " << port);

                io_context.run();

                SPARK_INFO("[TCP SERVER STOPPED]: [IP]: " << ip << " [PORT]: " << port);
            }

            ~tcp_server()
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
            void accept_connection()
            {
                m_acceptor.async_accept(m_socket,
                    [this](boost::system::error_code ec)
                    {
                        if (!ec)
                        {
                            start_receive();
                        }
                        accept_connection();
                    });
            }

            void start_receive()
            {
                m_socket.async_read_some(
                    asio::buffer(m_receive_buffer),
                    [this](boost::system::error_code ec, uint64_t bytes_recvd)
                    {
                        if (!ec && bytes_recvd > 0)
                        {
                            try
                            {
                                auto [type, version, data] = deserialize(std::string(m_receive_buffer.begin(), m_receive_buffer.begin() + bytes_recvd));
                                auto packet = packet_factory_registry::create_packet(type, version, data);
                                SPARK_TRACE("[TCP SERVER RECEIVED PACKET]: [TYPE]:" << type << " [VERSION]:" << std::to_string(version));
                                if (packet)
                                {
                                    packet->process();
                                }
                                else
                                {
                                    SPARK_ERROR("[TCP SERVER UNABLE TO PROCESS PACKET]: [TYPE]:" << type << " [VERSION]:" << std::to_string(version));
                                }
                            }
                            catch (const std::exception& e)
                            {
                                std::cerr << "Error processing packet: " << e.what() << std::endl;
                            }

							start_receive();
                        }
                    });
            }

            asio::io_context& m_io_context;
            tcp::acceptor m_acceptor;
            tcp::socket m_socket;
            std::array<uint8_t, 4096> m_receive_buffer;
        };
    }
}

#endif
