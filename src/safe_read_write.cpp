#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <type_traits>
#include <iostream>

// TODO: terminal 
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


int main()
{
  int data = 0;
  SafeReadWrite<int> wrapper(data);
  bool start = false;
  bool end = false;
  int sum = 0;

  std::thread write_thread([&]{
      while (!start) {}
      for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        wrapper.set(i);
        std::cout << "Write thread set " << i << std::endl;
      }
      end = true;
  });
  std::thread read_thread([&]{
      while (!start) {}
      while (!end) {
        // auto temp = wrapper.get();
        // std::cout << "Read thread get " << *temp << std::endl;
        // sum += *temp;
        int temp = 0;
        wrapper.get(temp);
        std::cout << "Read thread get " << temp << std::endl;
        sum += temp;
      }
  });

  start = true;
  write_thread.join();
  read_thread.join();
  std::cout << sum << std::endl;

  return 0;
}
