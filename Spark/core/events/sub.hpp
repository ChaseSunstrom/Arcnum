#ifndef SPARK_SUB_HPP
#define SPARK_SUB_HPP

#include "../spark.hpp"
#include "event.hpp"

namespace spark
{
	static uint64_t subscription_count = 0;

	// Macros instead of enums to allow for any subscription topic
#define NO_EVENT_TOPIC 0
#define EVERY_EVENT_TOPIC 1
#define WINDOW_EVENT_TOPIC 2
#define RENDERER_EVENT_TOPIC 3
#define APP_EVENT_TOPIC 4
#define INPUT_EVENT_TOPIC 5

	struct base_subscription
	{
		base_subscription(int32_t topic) :
			m_topic(topic), m_subscription_id(subscription_count++) { }

		virtual ~base_subscription() = default;

		virtual void publish(int32_t topic, std::shared_ptr<event> value) = 0;

		virtual void unsubscribe() = 0;

		uint64_t m_subscription_id;

		int32_t m_topic;
	};

	inline std::vector <std::shared_ptr<base_subscription>> subscriptions =
		std::vector<std::shared_ptr<base_subscription >>();

	template <typename T>
	struct subscription : base_subscription
	{
		static std::shared_ptr <subscription<T>> create(int32_t topic, std::function<void(std::shared_ptr<T >)> cb)
		{
			auto sub = std::shared_ptr<subscription<T>>(new subscription(topic, cb));
			subscriptions.push_back(std::static_pointer_cast<base_subscription>(sub));
			return sub;
		}

		inline void publish(int32_t topic, std::shared_ptr<event> value) override
		{
			if (topic == m_topic)
			{
				callback(value);
			}
		}

		inline void unsubscribe() override
		{
			auto it = std::find_if(
				subscriptions.begin(), subscriptions.end(), [this](const std::shared_ptr <base_subscription>& sub)
				{
					return sub->m_subscription_id == this->m_subscription_id;
				});

			if (it != subscriptions.end())
			{
				subscriptions.erase(it);
			}
		}

		std::function<void(std::shared_ptr<T>)> callback;

	private:
		// Make the constructor private to force usage of the factory method
		subscription(int32_t topic, std::function<void(std::shared_ptr<T >)> cb) :
			base_subscription(topic), callback(cb) { }
	};

	// Function to publish events to subscriptions of a specific topic.
	void publish_to_topic(int32_t topic, std::shared_ptr<event> value);
}

#endif