#ifndef SPARK_SUB_HPP
#define SPARK_SUB_HPP

#include "../spark.hpp"
#include "event.hpp"

namespace spark
{
	static u64 subscription_count = 0;

	struct ISubscription
	{
		ISubscription(i32 topic) :
				m_topic(topic), m_subscription_id(subscription_count++) { }

		virtual ~ISubscription() = default;

		virtual void publish(i32 topic, std::shared_ptr <Event> value) = 0;

		virtual void unsubscribe() = 0;

		u64 m_subscription_id;

		i32 m_topic;
	};

	inline std::vector <std::shared_ptr<ISubscription>> subscriptions =
			std::vector < std::shared_ptr < ISubscription >> ();

	template <typename T>
	struct Subscription :
			ISubscription
	{
		static std::shared_ptr <Subscription<T>> create(i32 topic, std::function<void(std::shared_ptr < T > )> cb)
		{
			auto sub = std::shared_ptr<Subscription<T>>(new Subscription(topic, cb));
			subscriptions.push_back(std::static_pointer_cast<ISubscription>(sub));
			return sub;
		}

		inline void publish(i32 topic, std::shared_ptr <Event> value) override
		{
			if (topic == m_topic)
			{
				callback(value);
			}
		}

		inline void unsubscribe() override
		{
			auto it = std::find_if(
					subscriptions.begin(), subscriptions.end(), [this](const std::shared_ptr <ISubscription>& sub)
					{
						return sub->m_subscription_id == this->m_subscription_id;
					});

			if (it != subscriptions.end())
			{
				subscriptions.erase(it);
			}
		}

		std::function<void(std::shared_ptr < T > )> callback;

	private:
		// Make the constructor private to force usage of the factory method
		Subscription(i32 topic, std::function<void(std::shared_ptr < T > )> cb) :
				ISubscription(topic), callback(cb) { }
	};

	// Function to publish events to subscriptions of a specific topic.
	void publish_to_topic(i32 topic, std::shared_ptr <Event> value);
}

#endif