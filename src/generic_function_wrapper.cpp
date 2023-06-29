#include <any>
#include <functional>
#include <map>
#include <string>
#include <iostream>

// reference: https://stackoverflow.com/questions/45715219/store-functions-with-different-signatures-in-a-map
template<typename Ret>
struct AnyCallable
{
    AnyCallable() {}
    template<typename F>
    AnyCallable(F&& fun) : AnyCallable(std::function(std::forward<F>(fun))) {}
    template<typename ... Args>
    AnyCallable(std::function<Ret(Args...)> fun) : m_any(fun) {}
    template<typename ... Args>
    Ret operator()(Args&& ... args) 
    { 
        return std::invoke(std::any_cast<std::function<Ret(Args...)>>(m_any), std::forward<Args>(args)...); 
    }
    std::any m_any;
};

void foo(int x, int y)
{
    std::cout << "foo" << x << y << std::endl;
}

void bar(std::string x, int y, int z)
{
    std::cout << "bar" << x << y << z << std::endl;
} 

using namespace std::literals;

int main()
{
    std::map<std::string, AnyCallable<void>> map;
    
    map["foo"] = &foo;      //store the methods in the map
    map["bar"] = &bar;
    
    map["foo"](1, 2);       //call them with parameters I get at runtime
    map["bar"]("Hello, std::string literal"s, 1, 2);
    try {
        map["bar"]("Hello, const char *literal", 1, 2); // bad_any_cast
    } catch (std::bad_any_cast&) {
        std::cout << "mismatched argument types" << std::endl;
    }
    map["bar"].operator()<std::string, int, int>("Hello, const char *literal", 1, 2); // explicit template parameters
    
    return 0;
}