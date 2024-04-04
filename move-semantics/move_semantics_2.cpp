#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <utility>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

using Helpers::Gadget;
using namespace std::literals;

namespace Explain
{
    template <typename T>
    class unique_ptr
    {
    public:
        unique_ptr() noexcept
            : ptr_{nullptr}
        { }

        unique_ptr(nullptr_t) noexcept : ptr_{nullptr}
        { } 

        explicit unique_ptr(T* ptr) noexcept
            : ptr_{ptr}
        { }

        // copy semantics - disabled
        unique_ptr(const unique_ptr& other) = delete;
        unique_ptr& operator=(const unique_ptr& other) = delete;

        // move semantics
        // unique_ptr(unique_ptr&& other) : ptr_{other.ptr_}
        // {
        //     other.ptr_ = nullptr;
        // }

        // move constructor
        unique_ptr(unique_ptr&& other) noexcept
            : ptr_{std::exchange(other.ptr_, nullptr)}
        { }

        // move assignment operator
        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            if (this != &other)
            {
                delete ptr_;

                // ptr_ = other.ptr_;
                // other.ptr_ = nullptr;
                ptr_ = std::exchange(other.ptr_, nullptr);
            }

            return *this;
        }

        ~unique_ptr() noexcept
        {
            delete ptr_;
        }

        explicit operator bool() const noexcept // conversion from unique_ptr<T> to bool
        {
            return ptr_ != nullptr;
        }

        T& operator*() const noexcept
        {
            return *ptr_;
        }

        T* operator->() const noexcept
        {
            return ptr_;
        }

        T* get() const noexcept
        {
            return ptr_;
        }

    private:
        T* ptr_;
    };
} // namespace Explain

Explain::unique_ptr<Gadget> create_gadget()
{
    static int id = 0;

    const int current_id = ++id;
    return Explain::unique_ptr<Gadget>(new Gadget(current_id, "Gadget#"s + std::to_string(current_id)));
}

TEST_CASE("move semantics - unique_ptr")
{
    SECTION("stack")
    {
        Gadget g1{42, "ipad"};
        g1.use();
    }

    SECTION("heap")
    {
        SECTION("C++98")
        {
            Gadget* ptr_g = new Gadget(665, "ipod");
            ptr_g->use();

            delete ptr_g;
        }

        SECTION("since C++11")
        {
            SECTION("default constructor")
            {
                Explain::unique_ptr<Gadget> ptr_other_g;
                CHECK(ptr_other_g.get() == nullptr);
            }

            SECTION("implicit conversion from nullptr")
            {
                Explain::unique_ptr<Gadget> ptr_g = nullptr;
            }
        }

        SECTION("automatic clean-up")
        {
            Explain::unique_ptr<Gadget> ptr_g(new Gadget(777, "smartwatch"));
            ptr_g->use();
            (*ptr_g).use();

            Explain::unique_ptr<Gadget> other_ptr_g = std::move(ptr_g); // move-constructor
            other_ptr_g->use();

            other_ptr_g = Explain::unique_ptr<Gadget>(new Gadget(888, "Gadget"));
            other_ptr_g->use();

            other_ptr_g = create_gadget();
            other_ptr_g->use();

            std::vector<Explain::unique_ptr<Gadget>> gadgets;

            gadgets.push_back(create_gadget());
            gadgets.push_back(create_gadget());
            gadgets.push_back(create_gadget());
            gadgets.push_back(create_gadget());
            gadgets.push_back(std::move(other_ptr_g));

            auto another_ptr = std::move(gadgets[2]);

            for (const auto& ptr_g : gadgets)
            {
                if (ptr_g)
                    ptr_g->use();
            }
        }
    }
}
