#ifndef SPARK_SINGLETON_HPP
#define SPARK_SINGLETON_HPP

#include "../spark.hpp"

namespace Spark
{

template <typename T> class Singleton
{
  public:
    static T &get()
    {
        static T instance;
        return instance;
    }

    static T& initialize_instance()
    {
        return get();
    }

  protected:
    Singleton() = default;
    virtual ~Singleton() = default;

  private:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
};
}

#endif