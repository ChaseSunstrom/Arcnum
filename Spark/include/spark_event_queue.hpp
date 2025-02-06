#ifndef SPARK_EVENT_QUEUE_HPP
#define SPARK_EVENT_QUEUE_HPP

#include "spark_event.hpp"

namespace spark
{
    class Application;

    namespace detail
    {
        struct ISystem;
    }

    class SPARK_API EventQueue
    {
    public:
        EventQueue() = default;

        // Submit an Event<Ts...> into the queue
        template <typename... Ts>
        void SubmitEvent(const Event<Ts...>& ev)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto ptr = std::make_shared<Event<Ts...>>(ev);
            m_events.emplace(ptr);
        }

        template <typename T, typename... Args>
        void SubmitEvent(Args&&... args)
        {

            std::lock_guard<std::mutex> lock(m_mutex);
            auto ptr = std::make_shared<Event<T>>(T{std::forward<Args>(args)...});
            m_events.emplace(ptr);
        }

        /**
         * @brief Subscribe to any event that is or extends Event<Ts...>.
         *
         * The user callback receives `Event<Ts...>&`.
         * If the event in the queue has an active type in [Ts...], it calls the callback.
         */
        template <typename... Ts>
        void Subscribe(std::function<void(Event<Ts...>&)> callback)
        {

            std::lock_guard<std::mutex> lock(m_mutex);
            SubscriptionRecord rec;
            // For type-checking: store each typeid(Ti)
            (rec.type_list.push_back(std::type_index(typeid(Ts))), ...);

            rec.fn = [user_cb = std::move(callback), rec](IEvent* base_evt, std::type_index active_tid, void* active_ptr)
                {
                    // If active_tid is in rec.type_list => we build a partial event
                    bool found = false;
                    for (auto& ti : rec.type_list)
                    {
                        if (ti == active_tid)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        return;
                    }
                    // Build a partial Event<Ts...> with a no-op deleter
                    std::shared_ptr<Event<Ts...>> partial_evt = MakeSubEvent<Ts...>(active_tid, active_ptr);
                    if (partial_evt)
                    {
                        user_cb(*partial_evt);
                    }
                };

            m_subscriptions.push_back(std::move(rec));
        }

        // DispatchAll calls each subscription's callback for each event
        void DispatchAll(Application& app, std::vector<std::unique_ptr<detail::ISystem>>& event_systems);


        // Clear out events of any active type in [Ts...]
        template <typename... Ts>
        void ClearType()
        {

            std::lock_guard<std::mutex> lock(m_mutex);
            std::queue<std::shared_ptr<IEvent>> kept;
            while (!m_events.empty())
            {
                auto evt_ptr = m_events.front();
                m_events.pop();

                bool should_discard = false;
                evt_ptr->VisitActive([&](std::type_index tid, void*)
                    {
                        // If tid matches any of Ts...
                        bool found = false;
                        ((tid == std::type_index(typeid(Ts)) ? (found = true) : false) || ...);
                        if (found)
                        {
                            should_discard = true;
                        }
                    });

                if (!should_discard)
                {
                    kept.push(evt_ptr);
                }
            }
            m_events.swap(kept);
        }

    private:
        // We'll store each subscription in a record
        struct SubscriptionRecord
        {
            std::vector<std::type_index> type_list;
            // (IEvent*, active_tid, void*) => void
            std::function<void(IEvent*, std::type_index, void*)> fn;
        };

        std::queue<std::shared_ptr<IEvent>> m_events;
        std::vector<SubscriptionRecord> m_subscriptions;
        std::mutex m_mutex; // Protects all queue + subscription ops

    private:
        template <typename... ts>
        static std::shared_ptr<Event<ts...>> MakeSubEvent(std::type_index tid, void* raw_ptr)
        {
            auto new_evt = std::make_shared<Event<ts...>>();
            bool assigned = AssignVariantList<ts...>(tid, raw_ptr, *new_evt);
            if (!assigned)
            {
                return nullptr;
            }
            return new_evt;
        }

        template <typename... ts>
        static bool AssignVariantList(std::type_index tid, void* raw_ptr, Event<ts...>& evt)
        {
            bool assigned = false;
            ((tid == std::type_index(typeid(ts))
                ? (assigned = SetVariant<ts...>(evt, reinterpret_cast<ts*>(raw_ptr)))
                : false) || ...);
            return assigned;
        }

        template <typename... ts, typename t>
        static bool SetVariant(Event<ts...>& evt, t* typed_ptr)
        {
            if (!typed_ptr)
            {
                return false;
            }

            // Instead of using a no-op deleter, create a new, owned copy of the event data.
            std::shared_ptr<t> sp = std::make_shared<t>(*typed_ptr);
            evt = Event<ts...>(sp);
            return true;
        }

    };

} // namespace spark

#endif // SPARK_EVENT_QUEUE_HPP
