#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <utility>

////////////////////////////////////////////////////////////////////////////
// Data - class with copy & move semantics (user provided implementation)

using namespace Helpers;

class Data
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    Data(std::string name, std::initializer_list<int> list)
        : name_{std::move(name)}
        , size_{list.size()}
    {
        data_ = new int[list.size()];
        std::copy(list.begin(), list.end(), data_);

        std::cout << "Data(" << name_ << ")\n";
    }

    Data(const Data& other)
        : name_(other.name_)
        , size_(other.size_)
    {
        std::cout << "Data(" << name_ << ": cc)\n";
        data_ = new int[size_];
        std::copy(other.begin(), other.end(), data_);
    }

    Data& operator=(const Data& other)
    {
        Data temp(other); // cc
        swap(temp);

        std::cout << "Data=(" << name_ << ": cc)\n";

        return *this;
    }

    Data(Data&& other)
        : name_{std::move(other.name_)}
        , data_{std::exchange(other.data_, nullptr)}
        , size_{std::exchange(other.size_, 0)}
    {
        std::cout << "Data(" << name_ << ": mv)\n";
    }

    Data& operator=(Data&& other)
    {
        if (this != &other)
        {
            Data temp = std::move(other); // mv
            swap(temp);
        }

        std::cout << "Data=(" << name_ << ": mv)\n";

        return *this;
    }

    ~Data()
    {
        delete[] data_;
    }

    void swap(Data& other)
    {
        name_.swap(other.name_);
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    iterator begin()
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }
};

namespace ModernCpp
{
    class Data
    {
        std::string name_;
        std::vector<int> data_;

    public:
        using iterator = std::vector<int>::iterator;
        using const_iterator = std::vector<int>::const_iterator;

        Data(std::string name, std::initializer_list<int> list)
            : name_{std::move(name)}
            , data_(list)
        {
            std::cout << "Data(" << name_ << ")\n";
        }

        void swap(Data& other)
        {
            name_.swap(other.name_);
            data_.swap(other.data_);
        }

        iterator begin()
        {
            return data_.begin();
        }

        iterator end()
        {
            return data_.end();
        }

        const_iterator begin() const
        {
            return data_.begin();
        }

        const_iterator end() const
        {
            return data_.end();
        }
    };
} // namespace ModernCpp

Data create_data_set()
{
    Data ds{"data-set-one", {54, 6, 34, 235, 64356, 235, 23}};

    return ds;
}

TEST_CASE("Data & move semantics")
{
    Data ds1{"ds1", {1, 2, 3, 4, 5}};

    Data backup = ds1; // copy
    Helpers::print(backup, "backup");

    Data target = std::move(ds1);
    Helpers::print(target, "target");

    ds1 = create_data_set();
}

struct Container
{
    std::vector<std::string> items;

    Container(size_t size)
        : items(size)
    { }

    Container(std::initializer_list<std::string> lst)
        : items(lst.size())
    {
        std::copy(lst.begin(), lst.end(), items.begin());
    }
};

TEST_CASE("initializer list")
{
    Container c1(42);
    CHECK(c1.items.size() == 42);

    Container c2 = {"one"s, "two"s, "three"s};
    CHECK(c2.items == std::vector{"one"s, "two"s, "three"s});
}

struct HyperGadget
{
    const Data name;
    Data description;
    double price;

    HyperGadget(Data name, Data desc, double price)
        : name{std::move(name)}
        , description{std::move(desc)}
        , price{price}
    { }

    HyperGadget(const HyperGadget&) = default;
    HyperGadget& operator=(const HyperGadget&) = default;
    HyperGadget(HyperGadget&&) = default;
    HyperGadget& operator=(HyperGadget&&) = default;
    ~HyperGadget() { std::cout << "~HyperGadget\n"; }
};

TEST_CASE("HyperGadget")
{
    HyperGadget hg(Data{"name", {1, 2, 3}}, Data{"desc", {7, 8, 9, 10}}, 99.99);

    std::cout << "------\n";

    HyperGadget backup_hg = hg;

    std::cout << "------\n";

    HyperGadget target_h = std::move(hg);
}