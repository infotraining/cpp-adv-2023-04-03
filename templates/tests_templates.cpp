#include "helpers.hpp"

#include <array>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

using Helpers::Gadget;
using namespace std::literals;

template <typename T>
T maximum(T a, T b)
{
    return (a < b) ? b : a;
}

namespace Cpp20
{
    auto maximum(auto a, auto b)
    {
        return (a < b) ? b : a;
    }

    namespace IsTransformed
    {
        template <typename T1, typename T2>
        auto maximum(T1 a, T2 b)
        {
            return (a < b) ? b : a;
        }
    } // namespace IsTransformed
} // namespace Cpp20

// template specialization
// template <>
// const char* maximum(const char* a, const char* b)
// {
//     return std::strcmp(a, b) < 0 ? b : a;
// }

// overloading
const char* maximum(const char* a, const char* b)
{
    return std::strcmp(a, b) < 0 ? b : a;
}

template <typename T>
void compilation_phases(T a)
{
    // undeclared(42); // phase1 error
    some_function(a); // phase2 error - if some_function(T) is not defined
}

struct Id
{
    int value;

    bool operator<(const Id& rhs) const
    {
        return value < rhs.value;
    }

    friend void some_function(Id id)
    {
    }
};

TEST_CASE("compilation pahses")
{
    // compilation_phases("42"s);
    compilation_phases(Id{42});
}

TEST_CASE("function templates")
{
    CHECK(maximum<int>(42, 665) == 665);

    SECTION("type deduction")
    {
        CHECK(maximum(42, 665) == 665);       // T = int
        CHECK(maximum(42.1, 665.9) == 665.9); // T = double

        SECTION("ambiguous")
        {
            // CHECK(maximum(42.1, 665) == 665.9);
            CHECK(maximum<int>(42.1, 665) == 665);
            CHECK(maximum(42.1, static_cast<double>(12)) == 42.1);

            CHECK(maximum("abc"s, "def"s) == "def"s);

            auto result = maximum(Id{42}, Id{665});
            // CHECK(result == Id{665});

            const char* txt1 = "Ola";
            const char* txt2 = "Ala";
            CHECK(maximum(txt1, txt2) == "Ola"s);
        }
    }
}

namespace Exercise
{
    template <typename InputIterator, typename TValue>
    InputIterator find(InputIterator start, InputIterator stop, TValue value)
    {
        for (InputIterator it{start}; it != stop; ++it)
        {
            if (*it == value)
            {
                return it;
            }
        }

        return stop;
    }

    template <typename InputIterator, typename OutputIterator>
    void copy(InputIterator from_start, InputIterator from_stop, OutputIterator to_start)
    {
        for (auto it{from_start}; it != from_stop; ++it, to_start++)
        {
            *to_start = *it;
        }
    }
} // namespace Exercise

TEMPLATE_TEST_CASE("find", "[algorithm]", (std::array<int, 10>), (std::vector<int>), (std::vector<double>), (std::list<int>))
{
    TestType tab1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("value in range")
    {
        auto pos = Exercise::find(std::begin(tab1), std::end(tab1), 7);
        CHECK(pos != std::end(tab1));
        CHECK(*pos == 7);
    }

    SECTION("value not in range")
    {
        auto pos = Exercise::find(std::begin(tab1), std::end(tab1), 42);
        CHECK(pos == std::end(tab1));
    }
}

TEMPLATE_TEST_CASE("copy", "[algorithm]", (std::array<int, 10>), (std::vector<int>), (std::vector<double>), (std::list<int>))
{
    TestType tab1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    using T = TestType::value_type;

    std::vector<T> target(10);

    Exercise::copy(std::begin(tab1), std::end(tab1), target.begin());

    CHECK(std::equal(tab1.begin(), tab1.end(), target.begin()));
}

bool is_even(int n)
{
    return n % 2 == 0;
}

struct IsEven
{
    bool operator()(int n) const
    {
        return n % 2 == 0;
    }
};

namespace Exercise
{
    template <typename InputIterator, typename Predicate>
    InputIterator find_if(InputIterator start, InputIterator stop, Predicate predicate)
    {
        for (InputIterator it{start}; it != stop; ++it)
        {
            if (predicate(*it))
            {
                return it;
            }
        }

        return stop;
    }
} // namespace Exercise

TEMPLATE_TEST_CASE("find_if", "[algorithm]", (std::vector<int>))
{
    SECTION("item found")
    {
        TestType vec = {665, 667, 42, 77};
        auto pos = Exercise::find_if(vec.begin(), vec.end(), is_even);
        CHECK(pos != vec.end());
        CHECK(*pos == 42);
    }

    SECTION("item not found")
    {
        TestType vec = {665, 667, 77};
        auto pos = Exercise::find_if(vec.begin(), vec.end(), IsEven{});
        CHECK(pos == vec.end());
    }

    SECTION("with lambda")
    {
        TestType vec = {665, 667, 42, 77};
        auto pos = Exercise::find_if(vec.begin(), vec.end(), [](int n) { return n % 2 == 0; });
        CHECK(pos != vec.end());
        CHECK(*pos == 42);
    }
}

TEST_CASE("class templates")
{
    // TODO
}

TEST_CASE("template aliases")
{
    // TODO
}

TEST_CASE("template variables")
{
    // TODO
}

TEST_CASE("lambdas")
{
    auto lambda = [](int a) { return a * a; };


    
}