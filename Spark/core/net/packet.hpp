#ifndef SPARK_PACKET_HPP
#define SPARK_PACKET_HPP

#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <memory>

#include "../spark.hpp"

namespace spark
{
    namespace net {
        class packet 
        {
        public:
            virtual ~packet() {}
            virtual void process() const = 0;

            template<class Archive>
            void serialize(Archive& archive, const uint32_t version) 
            {
                // Empty for base class, but necessary for serialization
            }
        private:
            friend class boost::serialization::access;
        };

        template<typename T>
        inline std::string serialize(const T& obj) 
        {
            std::ostringstream oss;
            boost::archive::text_oarchive oa(oss);
            oa << obj;
            return oss.str();
        }

        inline std::unique_ptr<packet> deserialize(const std::string& data) 
        {
            std::istringstream iss(data);
            boost::archive::text_iarchive ia(iss);
            std::unique_ptr<packet> obj;
            ia >> obj;
            return obj;
        }

        BOOST_SERIALIZATION_ASSUME_ABSTRACT(packet);
	}
}

#endif