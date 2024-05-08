#ifndef SPARK_PACKET_HPP
#define SPARK_PACKET_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/preprocessor.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/stack.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>

#include "../logging/log.hpp"
#include "../spark.hpp"

#define DECLARE_STATE_MEMBER(r, data, i, elem) decltype(data::elem) elem;

#define BOOST_SERIALIZATION_NVP_EACH(r, data, i, elem)                         \
  ar &boost::serialization::make_nvp(BOOST_PP_STRINGIZE(elem), elem);

#define ASSIGN_FROM_CLASS(r, data, i, elem) state.elem = this->elem;

#define ASSIGN_FROM_STATE(r, data, i, elem) this->elem = state.elem;

// Main serialization and state handling macro
#define SERIALIZE_MEMBERS(class_name, ...)                                     \
  friend class boost::serialization::access;                                   \
  template <class Archive>                                                     \
  void serialize(Archive &ar, const unsigned int version) {                    \
    BOOST_PP_SEQ_FOR_EACH_I(BOOST_SERIALIZATION_NVP_EACH, ~,                   \
                            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
  }                                                                            \
  struct class_name##_##state {                                                \
    BOOST_PP_SEQ_FOR_EACH_I(DECLARE_STATE_MEMBER, class_name,                  \
                            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
    friend class boost::serialization::access;                                 \
    friend class class_name;                                                   \
    template <class Archive>                                                   \
    void serialize(Archive &ar, const unsigned int version) {                  \
      BOOST_PP_SEQ_FOR_EACH_I(BOOST_SERIALIZATION_NVP_EACH, ~,                 \
                              BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))           \
    }                                                                          \
  };                                                                           \
  std::string export_state_to_binary() const {                                 \
    std::ostringstream oss;                                                    \
    {                                                                          \
      boost::archive::binary_oarchive oa(oss);                                 \
      oa << *this;                                                             \
    }                                                                          \
    return oss.str();                                                          \
  }                                                                            \
  void apply_state_from_binary(const std::string &serialized_state) {          \
    std::istringstream iss(serialized_state);                                  \
    {                                                                          \
      boost::archive::binary_iarchive ia(iss);                                 \
      ia >> *this;                                                             \
    }                                                                          \
  }

#define SERIALIZE_EMPTY()                                                      \
  friend class boost::serialization::access;                                   \
  template <class Archive>                                                     \
  void serialize(Archive &ar, const unsigned int version) {}                   \
  struct state {                                                               \
    friend class boost::serialization::access;                                 \
    template <class Archive>                                                   \
    void serialize(Archive &ar, const unsigned int version) {}                 \
  };                                                                           \
  std::string export_state_to_binary() const { return ""; }                    \
  void apply_state_from_binary(const std::string &serialized_state) {}

template <typename T> struct PacketTraits;

namespace Spark {
template <typename T> std::string serialize_to_binary(const T &obj) {
  std::ostringstream obj_stream;
  {
    boost::archive::binary_oarchive oa(obj_stream);
    oa << obj;
  }
  return obj_stream.str();
}

template <typename T> T deserialize_from_binary(const std::string &data) {
  T obj;
  std::istringstream obj_stream(data);
  {
    boost::archive::binary_iarchive ia(obj_stream);
    ia >> obj;
  }
  return obj;
}

class Serializeable {
protected:
  Serializeable(const std::string &type, uint16_t version)
      : m_type(type), m_version(version) {}

public:
  virtual ~Serializeable() = default;

  virtual void process() const = 0;

  const std::string &get_type() const { return m_type; }

  uint16_t get_version() const { return m_version; }

protected:
  std::string m_type;

  uint16_t m_version;

  SERIALIZE_MEMBERS(Serializeable, m_type, m_version)
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Serializeable);

template <typename T>
void save_to_file(const T &obj, const std::string &filepath) {
  std::ofstream file(filepath, std::ios::binary);

  if (!file.is_open()) {
    SPARK_ERROR("[ FILE SAVING ]: Failed to save to file: " << filepath);
  }

  boost::archive::binary_oarchive oa(file);
  oa << obj;
}

template <typename T> T load_from_file(const std::string &filepath) {
  // Open a file stream for reading in binary mode
  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) {
    SPARK_ERROR("[ FILE SAVING ]: Failed to save to file: " << filepath);
  }

  T obj;

  // Create a binary archive and deserialize the object from it
  boost::archive::binary_iarchive ia(file);
  ia >> obj;

  // Return the deserialized object
  return obj;
}

namespace net {
class IPacketFactory {
public:
  virtual std::unique_ptr<Serializeable> create(const std::string &data) = 0;
};

class PacketFactoryRegistry {
public:
  static void register_factory(const std::string &type, uint16_t version,
                               std::unique_ptr<IPacketFactory> factory) {
    std::string key = generate_key(type, version);
    SPARK_TRACE("[PACKET FACTORY REGISTERED]: " << key);
    get_factories()[key] = std::move(factory);
  }

  static std::unique_ptr<Serializeable> create_packet(const std::string &type,
                                                      uint16_t version,
                                                      const std::string &data) {
    std::string key = generate_key(type, version);
    auto &factory = get_factories();
    auto it = factory.find(key);
    if (it != factory.end()) {
      return it->second->create(data);
    }
    return nullptr;
  }

  static std::unordered_map<std::string, std::unique_ptr<IPacketFactory>> &
  get_factories() {
    static std::unordered_map<std::string, std::unique_ptr<IPacketFactory>>
        s_factories;
    return s_factories;
  }

private:
  static std::string generate_key(const std::string &type, uint16_t version) {
    return type + "_" + std::to_string(version);
  }

private:
  static std::unordered_map<std::string, std::unique_ptr<IPacketFactory>>
      s_factories;
};

struct Envelope {
  Envelope() = default;

  Envelope(const std::string &type, uint16_t version, const std::string &data)
      : m_type(type), m_version(version), m_data(data) {}

  std::string m_type;

  uint16_t m_version;

  std::string m_data;

  SERIALIZE_MEMBERS(Envelope, m_type, m_version, m_data)
};

template <typename T> std::string serialize(const T &obj) {
  // Serialize the object into a string stream
  std::ostringstream obj_stream;
  {
    boost::archive::text_oarchive oa(obj_stream);
    oa << obj;
  }

  Envelope env(obj.get_type(), obj.get_version(), obj_stream.str());

  // Serialize the envelope into a string stream
  std::ostringstream env_stream;
  {
    boost::archive::text_oarchive oa(env_stream);
    oa << env;
  }

  // Return the serialized envelope as a string
  return env_stream.str();
}

inline std::tuple<std::string, uint16_t, std::string>
deserialize(const std::string &data) {
  std::istringstream iss(data);
  ::boost::archive::text_iarchive ia(iss);
  Envelope env;
  ia >> env;
  return std::make_tuple(env.m_type, env.m_version, env.m_data);
}

using Packet = Serializeable;

// Macro to register a packet type and its traits
#define REGISTER_PACKET_TYPE(NAMESPACE, PACKET_TYPE, VERSION)                  \
  BOOST_CLASS_EXPORT_GUID(NAMESPACE::PACKET_TYPE, #PACKET_TYPE)                \
  template <> struct ::PacketTraits<NAMESPACE::PACKET_TYPE> {                  \
    static const std::string type() { return #PACKET_TYPE; }                   \
    static const uint16_t version() { return VERSION; }                        \
  };                                                                           \
  class PACKET_TYPE##Factory : public ::Spark::net::IPacketFactory {           \
  public:                                                                      \
    std::unique_ptr<::Spark::net::Packet>                                      \
    create(const std::string &data) override {                                 \
      auto p = std::make_unique<NAMESPACE::PACKET_TYPE>();                     \
      std::istringstream iss(data);                                            \
      boost::archive::text_iarchive ia(iss);                                   \
      ia >> *p;                                                                \
      return p;                                                                \
    }                                                                          \
  };                                                                           \
  static bool registered_##PACKET_TYPE = []() {                                \
    ::Spark::net::PacketFactoryRegistry::register_factory(                     \
        PacketTraits<NAMESPACE::PACKET_TYPE>::type(),                          \
        PacketTraits<NAMESPACE::PACKET_TYPE>::version(),                       \
        std::make_unique<PACKET_TYPE##Factory>());                             \
    return true;                                                               \
  }();
} // namespace net
} // namespace spark

#endif