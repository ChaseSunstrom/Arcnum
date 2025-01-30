#ifndef SPARK_EVENT_QUEUE_HPP
#define SPARK_EVENT_QUEUE_HPP

#include "spark_event.hpp"

namespace spark
{
    class EventQueue
    {
    public:
        //-----------------------------------------------------
        // Submit any Event<Ts...> to the queue
        //-----------------------------------------------------
        template <typename... Ts>
        void SubmitEvent(const Event<Ts...>& ev)
        {
            // store as IEvent*
            auto ptr = std::make_shared<Event<Ts...>>(ev);
            m_events.push(ptr);
        }

        /**
         * @brief A single Subscribe<Ts...> method that handles:
         * - If Ts... has 1 type => single-type subscription
         * - If Ts... has multiple => "any-of" subscription:
         *   fires if the event's active data is any of [Ts...].
         *
         * The user callback receives an Event<Ts...>& (a "partial" event
         * built at runtime with whichever pointer matched).
         */
        template <typename... Ts>
        void Subscribe(std::function<void(Event<Ts...>&)> callback)
        {
            // We'll build a record with a list of [typeid(Ts)...],
            // plus a function that (1) checks if active data is in that set,
            // (2) if yes, builds a partial `Event<Ts...>` with the pointer,
            // (3) calls the user's callback with it.
            SubscriptionRecord rec;
            // Populate rec.type_list: we store each typeid(Ti)
            (rec.type_list.push_back(std::type_index(typeid(Ts))), ...);

            // Create an erased function that will run on each event->VisitActive
            rec.fn = [user_cb = std::move(callback), rec](IEvent* base_evt,
                std::type_index active_tid,
                void* active_ptr)
                {
                    // Check if active_tid matches any in rec.type_list
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
                        return; // not relevant for this subscription

                    // We create a new partial event< Ts... >
                    // that has the pointer for whichever type matched.
                    auto partial_evt = MakeSubEvent<Ts...>(active_tid, active_ptr);
                    if (partial_evt)
                    {
                        // call user callback
                        user_cb(*partial_evt);
                    }
                };

            m_subscriptions.push_back(std::move(rec));
        }

        //-----------------------------------------------------
        // Process (dispatch) all events
        //-----------------------------------------------------
        void DispatchAll()
        {
            while (!m_events.empty())
            {
                auto evt_ptr = m_events.front();
                m_events.pop();

                // For each event, get (active_tid, active_ptr) via VisitActive
                evt_ptr->VisitActive([this, base_evt = evt_ptr.get()](std::type_index tid, void* ptr)
                    {
                        // For each subscription, run if we match
                        for (auto& sub : m_subscriptions)
                        {
                            sub.fn(base_evt, tid, ptr);
                        }
                    });
            }
        }

        template <typename... Ts>
        void ClearType()
        {
            // We'll build a temporary queue of "kept" events
            std::queue<std::shared_ptr<IEvent>> kept;

            // Move items from m_events => check => either discard or push to 'kept'
            while (!m_events.empty())
            {
                auto evt_ptr = m_events.front();
                m_events.pop();

                // We'll see if it matches any data type in [Ts...]
                bool should_discard = false;

                // Check the event's active data type
                evt_ptr->VisitActive([&](std::type_index tid, void* ptr)
                    {
                        // If tid matches any of [Ts...], we mark it for discard
                        bool found = false;
                        // initializer-list trick or simple loop
                        // We'll do a small loop approach:

                        // Expand type pack [Ts...]
                        bool dummy[] = { (tid == std::type_index(typeid(Ts)) ? (found = true) : false)... };
                        (void)dummy;

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

            // Finally, swap back
            m_events.swap(kept);
        }


    private:
        // A queue of events
        std::queue<std::shared_ptr<IEvent>> m_events;

        // Each subscription = { [list of typeids], function(...) }
        struct SubscriptionRecord
        {
            std::vector<std::type_index> type_list;
            // Function arguments: (base_evt, active_tid, active_ptr)
            std::function<void(IEvent*, std::type_index, void*)> fn;
        };
        std::vector<SubscriptionRecord> m_subscriptions;

    private:
        // Builds a partial Event<Ts...> if tid matches one of Ts...
        // storing the pointer active_ptr in the variant
        // (We create a no-op deleter so we don't double-free.)
        template <typename... Ts>
        static std::shared_ptr<Event<Ts...>> MakeSubEvent(std::type_index tid, void* ptr)
        {
            // We'll build a new event with no pointer,
            // then try to set the pointer if tid matches one of Ts...
            auto new_evt = std::make_shared<Event<Ts...>>();
            bool assigned = AssignVariantList<Ts...>(tid, ptr, *new_evt);

            if (!assigned)
            {
                // means tid wasn't any of Ts..., or pointer was null
                return nullptr;
            }
            return new_evt;
        }

        // Tries each type in Ts... to see if tid == typeid(T).
        // If so, store that pointer in `evt`.
        template <typename... Ts>
        static bool AssignVariantList(std::type_index tid,
            void* raw_ptr,
            Event<Ts...>& evt)
        {
            bool assigned = false;

            // c++17 approach: an initializer list trick
            // For each type T in [Ts...], do a check:
            bool dummy[] = {
                ((tid == std::type_index(typeid(Ts)))
                   ? (assigned = SetVariant<Ts...>(evt, reinterpret_cast<Ts*>(raw_ptr)))
                   : false)...
            };
            (void)dummy; // avoid unused warning

            return assigned;
        }

        // Actually set the pointer in the event. We create a shared_ptr<T>
        // with a no-op deleter, then call the appropriate constructor
        // on `evt`. (Because `Event<Ts...>` can store one pointer in its variant.)
        template <typename... Ts, typename T>
        static bool SetVariant(Event<Ts...>& evt, T* typed_ptr)
        {
            if (!typed_ptr) return false;
            // build a no-op shared_ptr from raw pointer
            std::shared_ptr<T> sp(typed_ptr, [](T*) { /* no-op deleter */ });
            // Then create a new event with that pointer
            evt = Event<Ts...>(sp);
            return true;
        }
    };
}

#endif // SPARK_EVENT_QUEUE_HPP
