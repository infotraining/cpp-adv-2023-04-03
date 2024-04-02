#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace Exercise
{
    enum class Implementation {
        Generic,
        Optimized
    };

    template <typename InputIterator, typename OutputIterator>
    Implementation copy(InputIterator start, InputIterator end, OutputIterator dest)
    {
        for (auto it = start; it != end; ++it, ++dest)
        {
            *dest = *it;
        }

        return Implementation::Generic;
    }
} // namespace Exercise

// TODO - optimized version with memcpy of copy

TEST_CASE("copy algorithm")
{
    using Exercise::Implementation;
    using std::begin, std::end;

    SECTION("generic version for STL containers")
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        std::list<int> lst(5);

        REQUIRE(Exercise::copy(vec.begin(), vec.end(), lst.begin()) == Implementation::Generic);
        REQUIRE(std::equal(vec.begin(), vec.end(), lst.begin(), lst.end()));
    }

    SECTION("generic for array of strings")
    {
        const std::string words[] = {"1", "2", "3"};
        std::string dest[3];

        REQUIRE(Exercise::copy(begin(words), end(words), begin(dest)) == Implementation::Generic);
        REQUIRE(std::equal(begin(words), end(words), begin(dest), end(dest)));
    }

    // SECTION("optimized for arrays of POD types")
    // {
    //     int tab1[5] = {1, 2, 3, 4, 5};
    //     int tab2[5];

    //     REQUIRE(Exercise::copy(begin(tab1), end(tab1), begin(tab2)) == Implementation::Optimized);
    //     REQUIRE(std::equal(begin(tab1), end(tab1), begin(tab2), end(tab2)));
    // }
}