#include <catch2/catch_test_macros.hpp>

int foo(int x)
{
    return 2 * x;
}

template <int V>
struct Foo
{
    static constexpr int value = 2 * V;
};

template <typename T>
struct Identity
{
    using type = T;
};

////////////////////////////////////////////////////////
// IsSame

template <typename T, T v>
struct IntegralConstant
{
    static constexpr T value = v;
};

static_assert(IntegralConstant<int, 42>::value == 42, "Error");

template <bool v>
using BoolConstant = IntegralConstant<bool, v>;

using TrueType = BoolConstant<true>;
using FalseType = BoolConstant<false>;

static_assert(BoolConstant<true>::value == true);
static_assert(BoolConstant<false>::value == false);
static_assert(TrueType::value == true);
static_assert(FalseType::value == false);

template <typename T1, typename T2>
struct IsSame : FalseType
{};
// {
//     static constexpr bool value = false;
// };

template <typename T>
struct IsSame<T, T> : TrueType
{};
// {
//     static constexpr bool value = true;
// };

////////////////////////////////////////////////////////////////////

template <typename T>
struct IsVoid : FalseType
{
    // static constexpr bool value = false; tak albo dziedziczenie
};

template <>
struct IsVoid<void> : TrueType
{};

template <typename T>
constexpr bool IsVoid_v = IsVoid<T>::value;

TEST_CASE("type traits")
{   
    CHECK(foo(2) == 4);
    CHECK(Foo<2>::value == 4);

    static_assert(IsSame<Identity<int>::type, int>::value, "Types are not the same");

    using X = int;
    using Y = void;

    static_assert(IsVoid<X>::value == false, "Error");
    static_assert(IsVoid_v<Y> == true, "Error");
}

template <typename T>
struct RemoveRef
{
    using type = T;
};

template <typename T>
struct RemoveRef<T&> : RemoveRef<T>
{
};

template <typename T>
struct RemoveRef<T&&> : RemoveRef<T>
{
};

template <typename T>
using RemoveRef_t = typename RemoveRef<T>::type;

TEST_CASE("removing reference from type")
{
    using U = int;
    using W = int&;

    typename RemoveRef<U>::type value1{};
    RemoveRef_t<W> value2{};
}