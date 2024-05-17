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

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

  protected:
    Singleton() = default;
    virtual ~Singleton() = default;

  private:
    struct Initializer
    {
        Initializer()
        {
            Singleton::get();
        }
    };

    static Initializer m_initializer;
};

template <typename T> typename Singleton<T>::Initializer Singleton<T>::m_initializer;

} // namespace Spark

#endif