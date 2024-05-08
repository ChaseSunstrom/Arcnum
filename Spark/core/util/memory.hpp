#ifndef SPARK_MEMORY_HPP
#define SPARK_MEMORY_HPP

#include "../logging/log.hpp"
#include "../spark.hpp"

namespace std {
template <typename TO, typename FROM>
unique_ptr<TO> static_pointer_cast(unique_ptr<FROM> &&old) {
  return unique_ptr<TO>((TO *)old.release());
}
} // namespace std

// Here because technically this is a smart pointer
namespace Spark {
template <typename T> class UniqueShieldPtr;

template <typename T> class UniqueProxy {
public:
  T *operator->() { return m_ptr; }

  T &operator*() { return *m_ptr; }

  UniqueProxy(const UniqueProxy &) = delete;

  UniqueProxy &operator=(const UniqueProxy &) = delete;

  ~UniqueProxy() = default;

private:
  friend class UniqueShieldPtr<T>;

  UniqueProxy(T *ptr, std::recursive_mutex &mtx) : m_ptr(ptr), m_lock(mtx) {}

  T *m_ptr;

  std::unique_lock<std::recursive_mutex> m_lock;
};

template <typename T> class UniqueShieldPtr {
public:
  explicit UniqueShieldPtr(T *ptr = nullptr)
      : m_data(ptr), m_mutex(new std::recursive_mutex) {}

  ~UniqueShieldPtr() {
    delete m_data;
    delete m_mutex;
  }

  // Delete copy semantics
  UniqueShieldPtr(const UniqueShieldPtr &) = delete;

  UniqueShieldPtr &operator=(const UniqueShieldPtr &) = delete;

  UniqueShieldPtr(UniqueShieldPtr &&other)

      noexcept
      : m_data(other.m_data), m_mutex(other

                                          .m_mutex) {
    other.m_data = nullptr;
    other.m_mutex = nullptr;
  }

  UniqueShieldPtr &operator=(UniqueShieldPtr &&other)

      noexcept {
    if (this != &other) {
      std::lock_guard<std::recursive_mutex> lock(
          *m_mutex); // Lock the current mutex
      delete m_data; // Safely delete the managed object

      std::lock_guard<std::recursive_mutex> other_lock(
          *other.m_mutex); // Lock the other mutex
      m_data = other.m_data;
      m_mutex = other.m_mutex;
      other.m_data = nullptr;
      other.m_mutex = nullptr;
    }
    return *this;
  }

  T *get() const { return m_data; }

  void reset(T *ptr = nullptr) {
    std::unique_lock<std::recursive_mutex> lock(*m_mutex);
    delete m_data;
    m_data = ptr;
  }

  UniqueProxy<T> access() {
    if (m_mutex) {
      return UniqueProxy<T>(m_data, *m_mutex);
    } else {
      SPARK_FATAL("unique_shield_ptr does not have a mutex");
    }
  }

private:
  T *m_data;

  std::recursive_mutex *m_mutex;
};

template <typename T> class SharedShieldPtr;

template <typename T> class SharedProxy {
public:
  T *operator->() { return m_ptr.get(); }

  T &operator*() { return *m_ptr.get(); }

  SharedProxy(const SharedProxy &) = delete;

  SharedProxy &operator=(const SharedProxy &) = delete;

  ~SharedProxy() = default;

private:
  friend class SharedShieldPtr<T>;

  SharedProxy(std::shared_ptr<T> ptr, std::recursive_mutex &mtx)
      : m_ptr(std::move(ptr)), m_lock(mtx) {}

  std::shared_ptr<T> m_ptr;

  std::unique_lock<std::recursive_mutex> m_lock;
};

template <typename T> class SharedShieldPtr {
public:
  explicit SharedShieldPtr(T *ptr = nullptr)
      : m_data(std::shared_ptr<T>(ptr)),
        m_mutex(std::make_shared<std::recursive_mutex>()) {}

  explicit SharedShieldPtr(std::shared_ptr<T> data,
                           std::shared_ptr<std::recursive_mutex> mutex)
      : m_data(std::move(data)), m_mutex(std::move(mutex)) {}

  SharedShieldPtr(const SharedShieldPtr &other) = default;

  SharedShieldPtr &operator=(const SharedShieldPtr &other) = default;

  SharedShieldPtr(SharedShieldPtr &&other) = default;

  SharedShieldPtr &operator=(SharedShieldPtr &&other) = default;

  T *get() const { return m_data.get(); }

  std::shared_ptr<T> get_shared_ptr() const { return m_data; }

  std::shared_ptr<std::recursive_mutex> get_mutex() const { return m_mutex; }

  void reset(T *ptr = nullptr) {
    std::unique_lock<std::recursive_mutex> lock(*m_mutex);
    m_data.reset(ptr);
  }

  SharedProxy<T> access() {
    if (m_mutex) {
      return SharedProxy<T>(m_data, *m_mutex);
    } else {
      SPARK_FATAL("shared_shield_ptr does not have a mutex");
    }
  }

  i32 use_count() const { return m_data.use_count(); }

private:
  std::shared_ptr<T> m_data;

  std::shared_ptr<std::recursive_mutex> m_mutex;
};

template <typename T, typename... Args>
UniqueShieldPtr<T> make_unique_shield(Args &&...args) {
  return UniqueShieldPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
SharedShieldPtr<T> make_shared_shield(Args &&...args) {
  return SharedShieldPtr<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

template <typename T>
UniqueShieldPtr<T> make_threaded(std::unique_ptr<T> &&ptr) {
  return UniqueShieldPtr<T>(ptr.release());
}

template <typename T> SharedShieldPtr<T> make_threaded(std::shared_ptr<T> ptr) {
  return SharedShieldPtr<T>(std::move(ptr),
                            std::make_shared<std::recursive_mutex>());
}

template <typename T, typename U>
UniqueShieldPtr<T> threaded_pointer_cast(const UniqueShieldPtr<U> &ptr) {
  return UniqueShieldPtr<T>((T *)ptr.get(), ptr.get_mutex());
}

template <typename T, typename U>
SharedShieldPtr<T> threaded_pointer_cast(const SharedShieldPtr<U> &ptr) {
  auto casted_ptr = std::static_pointer_cast<T>(ptr.get_shared_ptr());
  return SharedShieldPtr<T>(std::move(casted_ptr), ptr.get_mutex());
}
} // namespace spark

#endif