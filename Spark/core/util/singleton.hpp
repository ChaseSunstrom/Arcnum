#ifndef SPARK_Singleton_HPP
#define SPARK_Singleton_HPP

#include "../spark.hpp"

namespace Spark
{
template <typename T> class Singleton
{
  public:
    static T &get()
    {
        return *m_instance.get();
    }

  protected:
    Singleton()
    {
    } // Constructor needs to be accessible to derived classes.
    virtual ~Singleton()
    {
    }

  private:
    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

    static std::unique_ptr<T> create_instance()
    {
        return std::unique_ptr<T>(new T());
    }

    static std::unique_ptr<T> m_instance;

    static struct initializer
    {
        initializer()
        {
            Singleton<T>::m_instance = create_instance();
        }
    } m_initializer;
};
} // namespace Spark

#endif