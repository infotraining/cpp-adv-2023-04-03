#define ENABLE_MOVE
#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>

using namespace std::literals;

Helpers::Vector create_and_fill()
{
    using Helpers::Vector, Helpers::String;

    Vector vec;

    String str = "very, very, very, very, very, very, very, very, very, very, very, very, very, very, very, very long text";

    vec.push_back(str);

    vec.push_back(str + str); 

    vec.push_back("text"); 

    vec.push_back(str);  

    return vec;
}

TEST_CASE("move semantics motivation")
{
    Helpers::Vector vec = create_and_fill();

    Helpers::String::print_stats("Total");
}