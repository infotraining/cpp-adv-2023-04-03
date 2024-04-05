#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <deque>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using namespace Helpers;

////////////////////////////////////////////////////////
///  PERFECT FORWARDING

// layer-1

void have_fun(Gadget& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

void have_fun(const Gadget& cg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    cg.use();
}

void have_fun(Gadget&& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

namespace WithoutPerfectForwarding
{
    // layer-2
    void use(Gadget& g)
    {
        have_fun(g);
    }

    void use(const Gadget& g)
    {
        have_fun(g);
    }

    void use(Gadget&& g)
    {
        have_fun(std::move(g));
    }
} // namespace WithoutPerfectForwarding

namespace PerfectForwarding
{
    template <typename TGadget>
    void use(TGadget&& g)
    {
        // //if g is passed as lvalue 
        // have_fun(g);

        // //if  g is passed as rvalue
        // have_fun(std::move(g));

        have_fun(std::forward<TGadget>(g));
    }

    namespace Cpp20
    {
        void use(auto&& g)
        {
            have_fun(std::forward<decltype(g)>(g));
        }
    }
}

TEST_CASE("using gadget")
{
    Gadget g{1, "g"};
    const Gadget cg{2, "const g"};

    using WithoutPerfectForwarding::use;

    use(g);
    use(cg);
    use(Gadget{3, "temporary gadget"});
}


template <typename T>
void collapse(int& ref)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

template <typename T>
void universal_ref(T&& r)  // type deduction context
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}

namespace Cpp14
{
    auto universal_lambda = [](auto&& value) {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    };
}

TEST_CASE("reference collapsing")
{
    int x = 10;
    int& ref_x = x;
    collapse<int>(x);
    collapse<int&>(ref_x);
    collapse<int&&>(x);

    std::cout << "----------------\n";

    universal_ref<int&>(x);
    universal_ref<int&&>(10);

    std::cout << "----------------\n";

    universal_ref(x);
    universal_ref(42);

    std::cout << "----------------\n";
    int lvalue = 10;
    Cpp14::universal_lambda(lvalue);
    Cpp14::universal_lambda(42);

    auto&& uref1 = lvalue;  // int&
    static_assert(std::is_same<decltype(uref1), int&>::value, "uref1 is not int&");

    auto&& uref2 = 42;      // int&&
    static_assert(std::is_same<decltype(uref2), int&&>::value, "uref2 is not int&&");
}