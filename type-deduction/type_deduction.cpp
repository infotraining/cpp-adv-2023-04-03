#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

namespace Cpp20
{
    void deduce1(auto arg)
    {
        puts(__PRETTY_FUNCTION__);
    }
}

template <typename T>
void deduce2(T& arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void deduce3(T&& arg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

TEST_CASE("type deduction rules")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = x;
    int tab[10];
    const char* words[20];

    SECTION("Case 1")
    {
        deduce1(x);      // int
        deduce1(cx);     // int - const is removed
        deduce1(ref_x);  // int - & is removed
        deduce1(cref_x); // int - const and & are removed
        deduce1(tab);    // int* - array decays to pointer   
        deduce1(foo);    // void(*)(int) - function decays to function pointer

        std::integral auto a1 = x;      // int
        auto a2 = cx;     // int - const is removed
        auto a3 = ref_x;  // int - & is removed 
        auto a4 = cref_x; // int - const and & are removed
        auto a5 = tab;    // int* - array decays to pointer   
        auto a6 = foo;    // void(*)(int) - function decays to function pointer
        auto a7 = words;  // const char** - array of c-string decays to pointer to pointer to const char
    }

    SECTION("Case 2")
    {
        deduce2(x);      
        deduce2(cx);     
        deduce2(ref_x);  
        deduce2(cref_x); 
        deduce2(tab);    
        deduce2(foo);    

        auto& a1 = x;       // int&
        auto& a2 = cx;      // const int&
        auto& a3 = ref_x;   // int&
        auto& a4 = cref_x;  // const int&
        auto& a5 = tab;     // int(&a5)[10]
        auto& a6 = foo;     // void(&a6)(int)
    }

    SECTION("Case 3")
    {
        deduce3(x);              // deduce3<int&>(int&)
        deduce3(ref_x);          // deduce3<int&>(int&)
        deduce3(cref_x);         // deduce<const int&>(const int&)
        deduce3(tab);            // deduce3<int(&)[10]>(int(&)[10])
        deduce3(foo);            // deduce3<void (&)(int)>(void (&)(int))
        deduce3(cx);             // deduce3<const int&>(const int&)
        deduce3(string("text")); // deduce4<string>(string&&)

        auto&& a1 = x;      // int&
        auto&& a2 = ref_x;  // int&
        auto&& a3 = cref_x; // const int&
        auto&& a4 = tab;    // int(&a4)[10]
        auto&& a5 = foo;    // void (&)(int)
        auto&& a6 = cx;
        auto&& a7 = string("text");
    }
}

TEST_CASE("decltype(auto)")
{ 
}

/////////////////////////////////////////////////////////////
// std::begin, std::end - how it works

namespace Explain
{
    template <typename TContainer>
    auto begin(TContainer& container)
    {
        return container.begin();
    }

    template <typename TContainer>
    auto end(TContainer& container)
    {
        return container.end();
    }

    template <typename TContainer>
    auto size(TContainer& container)
    {
        return container.size();
    }

    template <typename T, size_t N>
    auto begin(T (&container)[N])
    {
        return &container[0];
    }

    template <typename T, size_t N>
    auto end(T (&container)[N])
    {
        return &container[0] + N;
    }

    template <typename T, size_t N>
    auto size(T (&container)[N])
    {
        return N;
    }
}

TEST_CASE("std::begin, std::end - how it works")
{
    int tab[10] = {1, 2, 3, 4};
    CHECK(Explain::size(tab) == 10);
    int (&ref_to_tab)[10] = tab;

    auto it = std::find(Explain::begin(tab), Explain::end(tab), 3);
    CHECK(*it == 3);
}

template <typename TContainer>
void zero(TContainer& container)
{
    using T = typename TContainer::value_type;

    for(auto&& item : container)
    {
        item = T{};
    }
}

TEST_CASE("range based for")
{
    std::vector<std::string> words = {"one", "two", "three"};
    for(auto& w : words)
    {
        w += "!";
        std::cout << w << " ";
    }
    std::cout << "\n";

    const std::vector<std::string> c_words = {"one", "two", "three"};
    for(auto& w : c_words)
    {
        std::cout << w << " ";
    }
    std::cout << "\n";

    std::vector<int> numbers = {1, 2, 3, 4};
    zero(numbers);
    CHECK(numbers == std::vector{0, 0, 0, 0});

    std::vector<bool> flags = {true, false, 0, 1};
    zero(flags);
    CHECK((flags == std::vector<bool>{0, 0, 0, 0}));
}

TEST_CASE("universal ref")
{
    int x = 10;
    auto&& uref_x = x; // int&

    auto&& uref_42 = 42;
}