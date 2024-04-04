#define ENABLE_MOVE_SEMANTICS
#define ENABLE_LOGGING_TO_CONSOLE
#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>

using namespace std::literals;

Helpers::Vector create_and_fill()
{
    using Helpers::Vector, Helpers::String;

    Vector vec;

    String str = "#";

    vec.push_back(str);

    std::cout << "--------------\n";

    vec.push_back(str + str); 

    std::cout << "--------------\n";

    vec.push_back("text1"); 

    std::cout << "--------------\n";
    
    vec.push_back("text2"); 

    std::cout << "--------------\n";
    
    vec.push_back("text3"); 

    std::cout << "--------------\n";

    vec.push_back(std::move(str));  

    return vec;
}

TEST_CASE("move semantics motivation")
{
    Helpers::Vector vec = create_and_fill();

    Helpers::String::print_stats("Total");
}