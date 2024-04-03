#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std::literals;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        SECTION("lvalue can be bound to lvalue-ref")
        {
            std::string& lref_name = name;

            lref_name = "Jan";
            CHECK(name == "Jan"s);
        }

        // std::string& lref_to_const_full_name = full_name(name, "Kowalski"); // ERROR

        SECTION("rvalue can bound to lvalue-ref to const")
        {
            const std::string& lref_to_const_full_name = full_name(name, "Kowalski");
            CHECK(lref_to_const_full_name == "jan Kowalski"s);
            // lref_to_const_full_name[0] = 'P';
        }
    }

    SECTION("C++11")
    {
        SECTION("rvalue can be bound to rvalue-ref")
        {

            std::string&& rvalue_ref_full_name = full_name(name, "Kowalski");
            CHECK(rvalue_ref_full_name == "jan Kowalski");
            rvalue_ref_full_name[0] = 'J';
            CHECK(rvalue_ref_full_name == "Jan Kowalski");
        }

        // std::string&& r_value_ref = name; // ERROR
    }
}

struct Person
{
    std::string name{};
    uint8_t age{};

    Person() = default; // user declared

    Person(const std::string& n, uint8_t age = 18)
        : name{n}
        , age{age}
    { }

    bool operator==(const Person& rhs) const
    {
        return this->name == rhs.name && this->age == rhs.age;
    }

    bool operator!=(const Person& rhs) const
    {
        return !(*this == rhs);
    }
};

TEST_CASE("special functions")
{
    Person p1{"Jan"};
    CHECK(p1.age == 18);

    Person p2{"Adam", 33};

    Person p3 = p2; // copy constructor
    CHECK(p3.name == "Adam");
    CHECK(p3.age == 33);
    CHECK(p2 == p3);
    CHECK(p1 != p3);

    Person p4 = std::move(p3);
    CHECK(p4 == Person{"Adam", 33});

    SECTION("it should never happened")
    {
        CHECK(p3.name == "");
        CHECK(p3.age == 33);
    }
}