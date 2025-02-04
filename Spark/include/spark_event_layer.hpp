#ifndef SPARK_EVENT_LAYER_HPP
#define SPARK_EVENT_LAYER_HPP

#include "spark_layer.hpp"
#include "spark_event_queue.hpp"
#include "spark_delta_time.hpp"

namespace spark
{
    /**
     * @brief A layer that processes (dispatches) events from a given EventQueue.
     *
     * On each update, we can call "DispatchAll" or partial logic.
     * We also expose a "ClearType<Ts...>()" so you can discard certain event types.
     */
    class EventLayer : public ILayer
    {
    public:
        // Construct with a reference (or pointer) to an existing queue
        EventLayer(EventQueue& event_queue)
	        : m_event_queue(event_queue)
    	{}

        virtual ~EventLayer() override = default;

        // Called once when layer is attached
        void OnAttach() override
        {
        }

        void OnDetach() override
        {
        }

        // Called every frame
        void OnUpdate(DeltaTime<f64> dt) override
        {
            m_event_queue.DispatchAll();
        }

        // If you want to explicitly clear out events of certain type(s)
        // from the queue at any moment, you can call this from code 
        // that has a pointer to the layer, or from another layer.
        template <typename... Ts>
        void ClearEventsOfType() const
        {
            m_event_queue.ClearType<Ts...>();
        }

    private:
        EventQueue& m_event_queue;
    };
}

#endif // SPARK_EVENT_LAYER_HPP
