#ifndef SPARK_PACKET_HPP
#define SPARK_PACKET_HPP

#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <BOOST/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "../spark.hpp"
#include "../logging/log.hpp"


template<typename T>
struct packet_traits;

namespace spark
{
	namespace net
	{
		class packet 
		{
		protected:
			packet(const std::string& type, uint16_t version)
				: m_type(type), m_version(version) {}

		public:
			virtual ~packet() = default;
			virtual void process() const = 0;

			const std::string& get_type() const { return m_type; }
			uint16_t get_version() const { return m_version; }

			template <class Archive>
			void serialize(Archive& ar, const unsigned int version) 
			{
				ar& m_type;
				ar& m_version;
			}
		protected:
				std::string m_type;
				uint16_t m_version;
		private:
			friend class ::boost::serialization::access;
		};

		BOOST_SERIALIZATION_ASSUME_ABSTRACT(packet);

		class base_packet_factory
		{
		public:
			virtual std::unique_ptr<packet> create(const std::string& data) = 0;
		};

		class packet_factory_registry
		{
		public:
			static void register_factory(const std::string& type, uint16_t version, std::unique_ptr<base_packet_factory> factory)
			{
				std::string key = generate_key(type, version);
				SPARK_TRACE("[PACKET FACTORY REGISTERED]: " << key);
				get_factories()[key] = std::move(factory);
			}

			static std::unique_ptr<packet> create_packet(const std::string& type, uint16_t version, const std::string& data)
			{
				std::string key = generate_key(type, version);
				auto& factory = get_factories();
				auto it = factory.find(key);
				if (it != factory.end())
				{
					return it->second->create(data);
				}
				return nullptr;
			}

			static std::unordered_map<std::string, std::unique_ptr<base_packet_factory>>& get_factories()
			{
				static std::unordered_map<std::string, std::unique_ptr<base_packet_factory>> s_factories;
				return s_factories;
			}

		private:
			static std::string generate_key(const std::string& type, uint16_t version)
			{
				return type + "_" + std::to_string(version);
			}
		private:
			static std::unordered_map<std::string, std::unique_ptr<base_packet_factory>> s_factories;
		};

		class envelope
		{
		public:
			envelope() = default;

			envelope(const std::string& type, uint16_t version, const std::string& data)
				: m_type(type), m_version(version), m_data(data) {}

			template<typename Archive>
			void serialize(Archive& oa, unsigned int version)
			{
				oa& m_type;
				oa& m_version;
				oa& m_data;
			}
		public:
			std::string m_type;
			uint16_t m_version;
			std::string m_data; // Serialized packet data
		private:
			friend class ::boost::serialization::access;
		};

		template<typename T>
		std::string serialize(const T& obj)
		{
			// Serialize the object into a string stream
			std::ostringstream obj_stream;
			{
				boost::archive::text_oarchive oa(obj_stream);
				oa << obj;
			}

			envelope env(obj.get_type(), obj.get_version(), obj_stream.str());

			// Serialize the envelope into a string stream
			std::ostringstream env_stream;
			{
				boost::archive::text_oarchive oa(env_stream);
				oa << env;
			}

			// Return the serialized envelope as a string
			return env_stream.str();
		}

		inline std::tuple<std::string, uint16_t, std::string> deserialize(const std::string& data) 
		{
			std::istringstream iss(data);
			::boost::archive::text_iarchive ia(iss);
			envelope env;
			ia >> env;
			return std::make_tuple(env.m_type, env.m_version, env.m_data);
		}

		// Macro to register a packet type and its traits
	#define REGISTER_PACKET_TYPE(NAMESPACE, PACKET_TYPE, VERSION) \
		BOOST_CLASS_EXPORT_GUID(NAMESPACE::PACKET_TYPE, #PACKET_TYPE) \
		template<> \
		struct ::packet_traits<NAMESPACE::PACKET_TYPE> { \
			static const std::string type() { return #PACKET_TYPE; } \
			static const uint16_t version() { return VERSION; } \
		}; \
		class PACKET_TYPE##_factory : public ::spark::net::base_packet_factory { \
		public: \
			std::unique_ptr<::spark::net::packet> create(const std::string& data) override { \
				auto p = std::make_unique<NAMESPACE::PACKET_TYPE>(); \
				std::istringstream iss(data); \
				boost::archive::text_iarchive ia(iss); \
				ia >> *p; \
				return p; \
			} \
		}; \
		static bool registered_##PACKET_TYPE = []() { \
			::spark::net::packet_factory_registry::register_factory( \
				packet_traits<NAMESPACE::PACKET_TYPE>::type(), \
				packet_traits<NAMESPACE::PACKET_TYPE>::version(), \
				std::make_unique<PACKET_TYPE##_factory>()); \
			return true; \
		}();
	}
}


#endif