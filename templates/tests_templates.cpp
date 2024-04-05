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

template <typename T, typename TContainer = std::vector<T>>
class Container
{
    TContainer items;

public:
    using iterator = typename TContainer::iterator;
    using const_iterator = typename TContainer::const_iterator;

    Container(size_t size = 0)
        : items(size)
    { }

    Container(std::initializer_list<T> lst)
        : items(lst.size())
    {
        std::copy(lst.begin(), lst.end(), items.begin());
    }

    template <typename U>
    void push_back(U&& item)
    {
        items.push_back(std::forward<U>(item)); // copy
    }

    template <typename... TArgs>
    void emplace_back(TArgs&&... args)
    {
        items.emplace_back(std::forward<TArgs>(args)...);
    }

    void push_back_by_value(T item)
    {
        std::cout << "void push_back(const std::string& item: " << item << ")\n";
        items.push_back(std::move(item)); // copy
    }

    iterator begin()
    {
        return items.begin();
    }

    iterator end()
    {
        return items.end();
    }

    const_iterator begin() const
    {
        return items.begin();
    }

    const_iterator end() const
    {
        return items.end();
    }
};

namespace TemplateTemplateParam
{
    template <typename T, template <typename, typename> class TContainer = std::vector>
    class Container
    {
        TContainer<T, std::allocator<T>> items;

    public:
        using iterator = typename TContainer<T, std::allocator<T>>::iterator;
        using const_iterator = typename TContainer<T, std::allocator<T>>::const_iterator;

        Container(size_t size = 0)
            : items(size)
        { }

        Container(std::initializer_list<T> lst)
            : items(lst.size())
        {
            std::copy(lst.begin(), lst.end(), items.begin());
        }

        template <typename U>
        void push_back(U&& item)
        {
            items.push_back(std::forward<U>(item)); // copy
        }

        template <typename... TArgs>
        void emplace_back(TArgs&&... args)
        {
            items.emplace_back(std::forward<TArgs>(args)...);
        }

        void push_back_by_value(T item)
        {
            std::cout << "void push_back(const std::string& item: " << item << ")\n";
            items.push_back(std::move(item)); // copy
        }

        iterator begin()
        {
            return items.begin();
        }

        iterator end()
        {
            return items.end();
        }

        const_iterator begin() const
        {
            return items.begin();
        }

        const_iterator end() const
        {
            return items.end();
        }
    };
} // namespace TemplateTemplateParam

template <typename T, size_t N>
struct Array
{
    T items[N];

    using iterator = T*;
    using const_iterator = const T*;

    size_t size() const
    {
        return N;
    }

    iterator begin()
    {
        return &items[0];
    }

    iterator end()
    {
        return begin() + N;
    }

    const_iterator begin() const
    {
        return &items[0];
    }

    const_iterator end() const
    {
        return begin() + N;
    }

    T& operator[](size_t index)
    {
        return items[index];
    }

    const T& operator[](size_t index) const
    {
        return items[index];
    }
};

TEST_CASE("class templates")
{
    TemplateTemplateParam::Container<std::string, std::list> container;

    container.push_back("text");
    container.emplace_back(10, '*');

    std::string text = "text";
    container.push_back(text);

    for (const auto& item : container)
    {
        std::cout << item << "\n";
    }

    std::cout << "--------------\n";

    Array<int, 10> arr = {1, 2, 3, 4};
    CHECK(arr.size() == 10);
    for (const auto& item : arr)
    {
        std::cout << item << "\n";
    }
}

constexpr double my_pi_d = 3.141592653589793238462643383279502884197;
constexpr float my_pi_f = 3.141592653589793238462643383279502884197;

template <typename T>
constexpr T my_pi(3.141592653589793238462643383279502884197);

TEST_CASE("template variables")
{
    std::cout << my_pi<double> << "\n";
    std::cout << my_pi<float> << "\n";
}

template <typename T>
struct ID
{
    const T value;

    ID(T value)
        : value{std::move(value)}
    { }

    // rule of five
    ID(const ID&) = default;
    ID& operator=(const ID&) = default;
    ID(ID&&) = delete;
    ID& operator=(ID&&) = delete;
    virtual ~ID() = default;

    const T& id() const
    {
        return value;
    }

    virtual void print() const
    {
        std::cout << "ID: " << value << "n";
    }
};

template <typename T>
struct SuperID : ID<T>
{
    using TBase = ID<T>;

    using TBase::TBase;

    void print() const override
    {
        std::cout << "SuperID: " << TBase::id() << "n";
    }
};

template <typename T>
struct Person : ID<T>
{
    std::string name;

    Person(T id, std::string name)
        : ID<T>{std::move(id)}
        , name{std::move(name)}
    { }
};

TEST_CASE("template & inheritance")
{
    Person<std::string> p1{"K4732", "Kowalski"};
    CHECK(p1.id() == "K4732");
}

class SuperGadget : public Gadget
{
public:
    // SuperGadget() = default;

    // SuperGadget(int id, const std::string& name = "unknown")
    //     : Gadget{id, name}
    // {
    // }

    using Gadget::Gadget; // constructor inheritance

    void super_use() const
    {
        std::cout << "SuperUsing: " << id() << " - " << name() << "\n";
    }
};

TEST_CASE("inheriting constructors")
{
    SuperGadget sg1;

    SuperGadget sg2{42, "ipad"};
    sg2.super_use();
}

///////////////////////////////////////////////////////////////////
// type dependent name

struct A
{
    static auto foo(int x) -> int // static method
    {
        return x;
    }

    struct bar // inner type
    {
        int value;

        bar(int v)
            : value{v}
        {
        }
    };
};

template <typename T>
void type_dependent_name()
{
    auto value1 = T::foo(42);
    auto value2 = typename T::bar(42);
}

namespace Ver_1
{
    template <typename T>
    typename T::value_type sum(const T& vec)
    {
        typename T::value_type sum{};

        for (const auto& e : vec)
        {
            sum += e;
        }

        return sum;
    }
} // namespace Ver_1

inline namespace Ver_2
{
    template <typename T>
    auto sum(const T& vec) -> std::decay_t<decltype(*std::begin(vec))> // const int
    {
        using ValueType = std::decay_t<decltype(*std::begin(vec))>;

        ValueType sum{};  // const int sum{};

        for (const auto& e : vec)
        {
            sum += e;
        }

        return sum;
    }
} // namespace Ver_1

TEST_CASE("type dependent names")
{
    auto v1 = A::foo(42);
    auto v2 = A::bar(42);

    type_dependent_name<A>();

    std::list<int> vec = {1, 2, 3, 4};

    int result = sum(vec);
    CHECK(result == 10);
}

namespace Lib
{
    namespace ver_1
    {
        int foo()
        {
            return 1;
        }
    }

    inline namespace ver_2
    {
        int foo()
        {
            return 2;
        }
    }
}

TEST_CASE("inline namespace")
{
    CHECK(Lib::foo() == 2);
    CHECK(Lib::ver_1::foo() == 1);
    CHECK(Lib::ver_2::foo() == 1);
}