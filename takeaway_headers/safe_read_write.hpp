#ifndef SAFE_READ_WRITE_HPP
#define SAFE_READ_WRITE_HPP

#include <memory>
#include <mutex>
#include <condition_variable>
#include <type_traits>

/**
 * @brief Safely update an object T between reading and writing threads
 * 
 * @tparam T 
 */
template <class T>
class SafeReadWrite
{
 public: 
  static_assert(std::is_copy_assignable<T>::value, "Given type is not copy assignable"); 
  static_assert(std::is_copy_constructible<T>::value, "Given type is not copy constructible"); 
  explicit SafeReadWrite(const T &); 
  explicit SafeReadWrite(T&&);
  ~SafeReadWrite() = default;

  SafeReadWrite() = delete;
  T& operator=(const T &) = delete;
  T& operator=(T&&) = delete;

  void set(const T &);
  void set(T&&);

  // data copied out 
  std::shared_ptr<T> get();
  void get(T &);
 private: 
  T data_;
  bool updated_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

template <class T>
SafeReadWrite<T>::SafeReadWrite(const T & data) : 
    data_(data), updated_(true)
{
}

template <class T>
SafeReadWrite<T>::SafeReadWrite(T&& data) :
    data_(data), updated_(true)
{
}

template <class T>
void SafeReadWrite<T>::set(const T &data)
{
  std::lock_guard<std::mutex> guard(mutex_);
  data_ = data;
  updated_ = true;
  cv_.notify_one();
}

template <class T>
void SafeReadWrite<T>::set(T&& data)
{
  std::lock_guard<std::mutex> guard(mutex_);
  data_ = data;
  updated_ = true;
  cv_.notify_one();
}

template <class T>
std::shared_ptr<T> SafeReadWrite<T>::get()
{
  std::unique_lock<std::mutex> lk(mutex_);
  cv_.wait(lk, [this]{ return updated_; });
  std::shared_ptr<T> result(std::make_shared<T>(data_));
  updated_ = false;
  return result;
}

template <class T>
void SafeReadWrite<T>::get(T &result)
{
  std::unique_lock<std::mutex> lk(mutex_);
  cv_.wait(lk, [this]{ return updated_; });
  result = data_;
  updated_ = false;
}

#endif // SAFE_READ_WRITE_HPP