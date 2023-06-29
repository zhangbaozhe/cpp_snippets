#include <memory>
#include <string>
#include <vector>
#include <array>
#include <iostream>


template <class T>
struct Wrapper
{
 public: 
  Wrapper() = delete;

  explicit Wrapper(T *obj_addr) : obj_ptr_(obj_addr)
  {
  }

  explicit Wrapper(const T &obj) : obj_ptr_(&obj)
  {
  }

  Wrapper(const Wrapper &) = default;
  Wrapper &operator=(const Wrapper &) = default;
  ~Wrapper() = default; 

  void doWork() {} 
 private: 
  T *obj_ptr_;
}; 

template <>
void Wrapper<int>::doWork()
{
  std::cout << "int specialization" << std::endl;
  std::cout << *obj_ptr_ << std::endl;
}

template <>
void Wrapper<std::string>::doWork()
{
  std::cout << "std::string specialization" << std::endl;
  std::cout << *obj_ptr_ << std::endl;
}

template <>
void Wrapper<std::vector<int>>::doWork()
{
  std::cout << "std::vector<int> specialization" << std::endl;
  for (const int &i : *obj_ptr_)
    std::cout << i << std::endl;
}


int main()
{
  std::shared_ptr<int> int_ptr = std::make_shared<int>(12);
  std::shared_ptr<std::string> str_ptr = 
      std::make_shared<std::string>("I'm string");
  std::shared_ptr<std::vector<int>> vec_int_ptr(new std::vector<int>({1, 2, 3, 4, 5}));
  Wrapper<int> int_wrapper(int_ptr.get());
  Wrapper<std::string> str_wrapper(str_ptr.get());
  Wrapper<std::vector<int>> vec_int_wrapper(vec_int_ptr.get());

  int_wrapper.doWork();
  str_wrapper.doWork();
  vec_int_wrapper.doWork();

  return 0;
}