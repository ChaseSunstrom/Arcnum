#include "sub.hpp"

namespace spark
{
	void publish_to_topic(int32_t topic, std::shared_ptr<event> value)
	{
		for (auto& sub: subscriptions)
		{
			// Publish to all subscriptions if the topic is EVERY_EVENT
			// or to specific topic subscriptions.
			if (sub->m_topic == EVERY_EVENT_TOPIC || sub->m_topic == topic)
			{
				sub->publish(sub->m_topic, value);
			}
		}
	}
}