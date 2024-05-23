#ifndef SPARK_UDP_HPP
#define SPARK_UDP_HPP

#include <boost/system/error_code.hpp>
#include <curl.h>

#include "../events/sub.hpp"
#include "../logging/log.hpp"
#include "../spark.hpp"
#include "../util/thread_pool.hpp"

#include "net_common.hpp"
#include "serializeable.hpp"

namespace Spark
{
namespace net
{
using boost::asio::ip::udp;
namespace asio = boost::asio;

class UDPClient
{
  public:
    UDPClient(const std::string &host = "127.0.0.1", const std::string &port = "8080",
              asio::io_context &io_context = default_io_context())
        : m_io_context(io_context), m_socket(io_context), m_resolver(io_context)
    {
        udp::resolver::query query(udp::v4(), host, port);
        m_endpoint = *m_resolver.resolve(query).begin();
        m_socket.open(udp::v4());
    }

    ~UDPClient()
    {
        if (m_socket.is_open())
        {
            m_socket.close();
        }
    }

    void start_receive()
    {
        m_socket.async_receive_from(
            asio::buffer(m_receive_buffer), m_sender_endpoint,
            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0)
                {
                    auto [type, version, data] =
                        deserialize(std::string(m_receive_buffer.begin(), m_receive_buffer.begin() + bytes_recvd));
                    auto packet = PacketFactoryRegistry::create_packet(type, version, data);
                    SPARK_TRACE("[UDP CLIENT RECEIVED PACKET]: [TYPE]:" << type
                                                                        << " [VERSION]:" << std::to_string(version));
                    if (packet)
                    {
                        packet->process();
                        std::shared_ptr<UDPServerReceiveEvent> event =
                            std::make_shared<UDPServerReceiveEvent>(std::move(packet));
                        // Publish the packet data to the event pipeline so other stuff
                        // can use the data the server recieves
                        publish_to_topic(UDP_CLIENT_RECEIVE_TOPIC, event);
                    }
                    else
                    {
                        SPARK_FATAL("[UDP CLIENT UNABLE TO PROCESS PACKET]: [TYPE]:" << type << " [VERSION]:"
                                                                                     << std::to_string(version));
                    }

                    start_receive();
                }
            });
    }

    // Call this method after initializing the client to start listening
    void run()
    {
        ThreadPool::enqueue(TaskPriority::HIGH, false, [this]() {
            start_receive();
            m_io_context.get().run();
        });
    }

    template <typename T> void send(const T &packet)
    {
        std::string serialized_packet = serialize(packet);
        m_socket.send_to(asio::buffer(serialized_packet), m_endpoint);
    }

  private:
    std::reference_wrapper<asio::io_context> m_io_context;

    udp::socket m_socket;

    udp::resolver m_resolver;

    udp::endpoint m_endpoint, m_sender_endpoint;

    std::array<uint8_t, 4096> m_receive_buffer;
};

class UDPServer
{
  public:
    UDPServer(const std::string &ip = "127.0.0.1", const std::string &port = "8080",
              asio::io_context &io_context = default_io_context())
        : m_socket(io_context), m_work(io_context), m_io_context(io_context)
    {
        udp::endpoint local_endpoint(asio::ip::make_address(ip), static_cast<uint16_t>(std::stoi(port)));

        m_socket.open(udp::v4());
        m_socket.bind(local_endpoint);

        SPARK_INFO("[UDP SERVER STARTED]: [IP]: " << ip << " [PORT]: " << port);
    }

    ~UDPServer()
    {
        if (m_socket.is_open())
        {
            m_socket.close();
        }
    }

    void run()
    {
        start_receive(); // Start receiving packets asynchronously
        m_io_context.get().run();
    }

    void close()
    {
        if (m_socket.is_open())
        {
            m_socket.close();
        }
    }

  private:
    void start_receive()
    {
        m_socket.async_receive_from(asio::buffer(m_receive_buffer), m_remote_endpoint,
                                    [this](boost::system::error_code ec, std::uint64_t bytes_recvd) {
                                        if (!ec && bytes_recvd > 0)
                                        {
                                            try
                                            {
                                                m_clients.insert(m_remote_endpoint);

                                                auto [type, version, data] = deserialize(std::string(
                                                    m_receive_buffer.begin(), m_receive_buffer.begin() + bytes_recvd));
                                                auto packet = PacketFactoryRegistry::create_packet(type, version, data);
                                                SPARK_TRACE("[UDP SERVER RECEIVED PACKET]: [TYPE]:"
                                                            << type << " [VERSION]:" << std::to_string(version));
                                                if (packet)
                                                {
                                                    packet->process();
                                                    broadcast(*packet);
                                                    std::shared_ptr<UDPServerReceiveEvent> event =
                                                        std::make_shared<UDPServerReceiveEvent>(std::move(packet));
                                                    // Publish the packet data to the event pipeline so other stuff
                                                    // can use the data the server recieves
                                                    publish_to_topic(UDP_SERVER_RECEIVE_TOPIC, event);
                                                }
                                                else
                                                {
                                                    SPARK_FATAL("[UDP SERVER UNABLE TO PROCESS PACKET]: [TYPE]:"
                                                                << type << " [VERSION]:" << std::to_string(version));
                                                }
                                            }
                                            catch (const std::exception &e)
                                            {
                                                std::cerr << "Error processing packet: " << e.what() << std::endl;
                                            }
                                            start_receive(); // Continue listening for incoming packets
                                        }
                                    });
    }

    void broadcast(const Serializeable &packet)
    {
        std::string serialized_packet = serialize(packet);

        for (const auto &client_endpoint : m_clients)
        {
            // Asynchronously send data to all clients except the sender
            m_socket.async_send_to(asio::buffer(serialized_packet), client_endpoint,
                                   // Need to move serialized_packet to keep it alive while we send it,
                                   // boost takes a reference to the packet data So this is necessary to
                                   // keep it alive
                                   [serialized_packet = std::move(serialized_packet),
                                    client_endpoint](boost::system::error_code ec, u64 bytes_sent) {
                                       if (ec)
                                       {
                                           SPARK_ERROR("[UDP SERVER BROADCAST]: Error sending packet to client: "
                                                       << ec.message() << "[CLIENT]: " << client_endpoint);
                                       }
                                       else
                                       {
                                           SPARK_TRACE("[UDP SERVER BROADCAST]: Broadcasted packet: "
                                                       << serialized_packet << "to [CLIENT]: " << client_endpoint);
                                       }
                                   });
        }
    }

  private:
    udp::socket m_socket;

    udp::endpoint m_remote_endpoint;

    std::array<uint8_t, 4096> m_receive_buffer;

    asio::io_context::work m_work;     // Ensure the io_context doesn't exit prematurely
    std::set<udp::endpoint> m_clients; // Conntected clients to the server
    std::reference_wrapper<asio::io_context> m_io_context;
};
} // namespace net
} // namespace Spark

#endif