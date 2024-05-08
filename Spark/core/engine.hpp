#ifndef SPARK_Engine_HPP
#define SPARK_Engine_HPP

namespace Spark {
class Engine {
public:
  // For getting class Singletons
  // THESE CLASSES MUST IMPLEMENT THE get() METHOD
  template <typename T> static T &get() { return T::get(); }
};
} // namespace spark

#endif