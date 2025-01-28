#ifndef SPARK_ASSET_HPP
#define SPARK_ASSET_HPP

#include "spark_serialization.hpp"

namespace spark {
    class IAsset {
    public:
        virtual ~IAsset() = default;
        virtual void Serialize(ISerializer* serializer) const = 0;
        virtual void Deserialize(IDeserializer* deserializer) = 0;
        virtual std::string GetTypeName() const = 0;
    };

} // namespace spark

#endif // SPARK_ASSET_BASE_HPP
