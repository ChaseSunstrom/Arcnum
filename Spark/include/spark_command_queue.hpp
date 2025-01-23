#ifndef SPARK_COMMAND_QUEUE_HPP
#define SPARK_COMMAND_QUEUE_HPP

#include "spark_pch.hpp"
#include "spark_layer.hpp"
#include "spark_command.hpp"

namespace spark
{
    class CommandQueue
    {
    public:
        // Use perfect forwarding for args
        template <ValidCommand T, typename... Args>
        void SubmitCommand(Args&&... args)
        {
            m_command_queue[typeid(T)].emplace(std::make_unique<T>(std::forward<Args>(args)...));
        }

        template <typename T>
        void FlushQueue(auto&& fn)
        {
            // Wrap the passed functor into one that accepts an ICommand&.
            // The wrapper performs a dynamic_cast and, if successful, calls the original functor.
            auto wrapper = [fn = std::forward<decltype(fn)>(fn)](ICommand& command)
                {
                    if (auto* concrete_command = dynamic_cast<T*>(&command))
                    {
                        fn(*concrete_command);
                    }
                    else
                    {
                    }
                };

            FlushAndRunQueue(typeid(T), wrapper);
        }

    private:
        void FlushAndRunQueue(const std::type_index& ti, const std::function<void(ICommand&)>& fn)
        {
            auto& queue = m_command_queue[ti];

            while (!queue.empty())
            {
                auto cmd = std::move(queue.front());
                queue.pop();

                // Execute the command using our wrapper function.
                cmd->Execute(fn);
            }
        }

    private:
        std::unordered_map<std::type_index, std::queue<std::unique_ptr<ICommand>>> m_command_queue;
    };

}


#endif