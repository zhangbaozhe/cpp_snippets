#include <memory>
#include <functional>
#include <map>
#include <string>
#include <iostream>

// reference: https://cplusplus.com/articles/oz18T05o/

template <typename Ret>
struct CallableObject
{
 private: 
  struct CallableObjectConcept 
  {
    // CallableObjectConcept() = default;
    virtual ~CallableObjectConcept() {}
    template <typename ... Args>
    Ret operator()(Args&& ... args); // not virtual, to be implemented
  }; 

  template <typename ... Args>
  struct CallableObjectModel : public CallableObjectConcept
  {
    // CallableObjectModel() = default;
    CallableObjectModel(std::function<Ret(Args...)> func) : func_(func) {}
    ~CallableObjectModel() override {}

    Ret operator()(Args&& ... args)
    {
      return func_(std::forward<Args>(args)...);
    }

   private: 
    std::function<Ret(Args...)> func_;
  }; 

  std::shared_ptr<CallableObjectConcept> callable_obj_concept_ptr_;

 public: 
  // CallableObject() = default; 
  // FIXME: if no default constructor provided 
  // std::map<k, CallableObject>[some key] will not be compiled, 
  // because [] returns a reference, which indicates a CallableObject
  // must be pre constructed (using the default constructor)
  // maybe use emplace

  // below only for C++17 or above 
  // template <typename F>
  // CallableObject(F &&func) : CallableObject(std::function(std::forward<F>(func))) {}

  template <typename ... Args>
  CallableObject(std::function<Ret(Args...)> func) : callable_obj_concept_ptr_(new CallableObjectModel<Args...>(func)) {}

   template <typename ... Args>
    Ret operator()(Args&& ... args)
    {
      return (*callable_obj_concept_ptr_)(std::forward<Args>(args)...);
    }
}; 

template <typename Ret>
template <typename ... Args>
Ret CallableObject<Ret>::CallableObjectConcept::operator()(Args&& ... args)
{
  return dynamic_cast<CallableObjectModel<Args...>&>(*this)(std::forward<Args>(args)...);
}


void foo(int x, int y)
{
    std::cout << "foo" << x << y << std::endl;
}

void bar(std::string x, int y, int z)
{
    std::cout << "bar" << x << y << z << std::endl;
} 


int main()
{
    std::map<std::string, CallableObject<void>> map;
    
    // reference: https://stackoverflow.com/questions/1935139/using-stdmapk-v-where-v-has-no-usable-default-constructor
    // provided that CallableObject<void> has a default constructor
    // map["foo"] = &foo;      //store the methods in the map
    // map["bar"] = &bar;

    map.insert({"foo", std::function<void(int, int)>(foo)});
    map.insert({"bar", std::function<void(std::string, int, int)>(bar)});
    
    map.at("foo")(1, 2);       //call them with parameters I get at runtime
    map.at("bar")(std::string("Hello, std::string literal"), 1, 2);
    
    return 0;
}