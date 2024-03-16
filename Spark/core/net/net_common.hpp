#ifndef SPARK_NET_COMMON_HPP
#define SPARK_NET_COMMON_HPP

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

	}
}

#endif