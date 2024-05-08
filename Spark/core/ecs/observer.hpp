#ifndef SPARK_OBSERVER_HPP
#define SPARK_OBSERVER_HPP

#include "../events/event.hpp"
#include "../spark.hpp"

namespace Spark {
class Observer {
public:
  explicit Observer(bool manual_register = false);

  virtual ~Observer();

  virtual void on_notify(std::shared_ptr<Event> event) {}

  void register_observer();

  bool m_manual_register;
};
} // namespace spark

#endif