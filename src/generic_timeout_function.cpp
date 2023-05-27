/*
 * @Author: Baozhe ZHANG 
 * @Date: 2023-05-27 20:59:03 
 * @Last Modified by: Baozhe ZHANG
 * @Last Modified time: 2023-05-27 22:21:38
 */

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>

struct Point
{
  double x;
  double y;
};

template <typename T, typename U>
struct Result
{
  T result;
  U status;
}; 

// reference https://stackoverflow.com/questions/40550730/how-to-implement-timeout-for-function-in-c
template <typename F, typename T, class... Args>
typename std::result_of<F&&(Args&&...)>::type 
run_with_timeout(F&& f, T timeout, Args&&... args)
{
  using R = typename std::result_of<F&&(Args&&...)>::type;
  std::packaged_task<R(Args...)> task(f);
  auto future = task.get_future();
  std::thread worker(std::move(task), std::forward<Args>(args)...);
  if (future.wait_for(timeout) != std::future_status::timeout) {
      worker.join();
      return future.get(); // this will propagate exception from f() if any
  } else {
      worker.detach(); // we leave the thread still running
      throw std::runtime_error("Timeout");
  }
}

template <typename F, typename T, class... Args>
Result<typename std::result_of<F&&(Args&&...)>::type, std::future_status>
run_with_timeout_with_status(F&& f, T timeout, Args&&... args)
{
  using R = typename std::result_of<F&&(Args&&...)>::type;
  std::packaged_task<R(Args...)> task(f);
  auto future = task.get_future();
  std::thread worker(std::move(task), std::forward<Args>(args)...);
  std::future_status status = future.wait_for(timeout);
  if (status != std::future_status::timeout) {
      worker.join();
      return Result<R, std::future_status>{future.get(), status}; // this will propagate exception from f() if any
  } else {
      worker.detach(); // we leave the thread still running
      // worker.~thread(); // see https://stackoverflow.com/questions/40550730/how-to-implement-timeout-for-function-in-c
      Result<R, std::future_status> result;
      result.status = status;
      return result;
  }
}


int main(int argc, char **argv)
{
  std::vector<double> vec;
  Point point_buffer[3];
  point_buffer[0] = Point{1, 1};
  point_buffer[1] = Point{2, 2};
  point_buffer[2] = Point{3, 3};

  auto func = 
      [](std::vector<double> &vec, Point *buffer, size_t size, double ratio) -> double
      {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        double result = 0.0;
        for (size_t i = 0; i < size; i++) {
          double norm = std::sqrt(buffer[i].x * buffer[i].x + 
              buffer[i].y * buffer[i].y);
          vec.push_back(norm);
          result += ratio * norm;
        }
        return result;
      };
    
    std::cout << "Result: " << run_with_timeout(func, std::chrono::seconds(3), std::ref(vec), &point_buffer[0], 3, 1.0) << std::endl;
    auto result = run_with_timeout_with_status(func, std::chrono::seconds(2), std::ref(vec), &point_buffer[0], 3, 1.0);
    std::cout << "Result: " << result.result << " " << "Status: " << (int)result.status << std::endl;

    
    for (const auto &e : vec)
        std::cout << "Element: " << e << std::endl;
}