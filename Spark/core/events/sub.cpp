#include "sub.hpp"

namespace Spark {
void publish_to_topic(i32 topic, std::shared_ptr<Event> value) {
  for (auto &sub : subscriptions) {
    // Publish to all subscriptions if the topic is EVERY_EVENT
    // or to specific topic subscriptions.
    if (sub->m_topic == EVERY_EVENT_TOPIC || sub->m_topic == topic) {
      sub->publish(sub->m_topic, value);
    }
  }
}
} // namespace spark