#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <iostream>
#include <string_view>
#include <vector>
#include <string>
#include <cstdint>

#include "gadget.hpp"

namespace Helpers
{
    using namespace std::literals;

    template <typename Container>
    void print(const Container& container, std::string_view prefix)
    {
        std::cout << prefix << ": [ ";
        for (const auto& item : container)
        {
            constexpr std::string_view str_prefix = std::is_convertible_v<decltype(item), std::string_view> ? "\""sv : ""sv;
            std::cout << str_prefix << item << str_prefix << " ";
        }
        
        std::cout << "]" << std::endl;
    }

    class String
    {
        std::uint64_t id_;
        std::string value_;

        static uint64_t gen_id()
        {

            return ++id_seed;
        }

        inline static std::uint64_t id_seed{};
        inline static std::uint64_t copy_constructor_count{};
        inline static std::uint64_t move_constructor_count{};
        inline static std::uint64_t copy_assignment_count{};
        inline static std::uint64_t move_assignment_count{};
        inline static bool silent_mode{false};

    public:
        static void print_stats(std::string_view msg = "")
        {
            std::cout << "==================================\n";
            std::cout << "-- " << (msg.empty() ? "" : msg) << "\n";
            std::cout << "----------------------------------\n";
            std::cout << "constructed: " << id_seed << "\n";
            std::cout << "copy constructed: " << copy_constructor_count << "\n";
            std::cout << "move constructed: " << move_constructor_count << "\n";
            std::cout << "copy assigned: " << copy_assignment_count << "\n";
            std::cout << "move assigned: " << move_assignment_count << "\n";
            std::cout << "==================================\n";
        }

        static void clear_stats()
        {
            id_seed = 0;
            copy_constructor_count = 0;
            copy_assignment_count = 0;
            move_constructor_count = 0;
            move_assignment_count = 0;
        }

        String()
            : id_{gen_id()}
            , value_{std::string("default") + std::to_string(id_)}
        {
            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(" << id_ << ", " << value_ << ")" << std::endl;
            #endif
        }

        String(const char* name)
            : id_{gen_id()}
            , value_{name}
        {
            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(" << id_ << ", " << value_ << ")" << std::endl;
            #endif
        }

        String(const std::string& name)
            : id_{gen_id()}
            , value_{name}
        {
            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(" << id_ << ", " << value_ << ")" << std::endl;
            #endif
        }

        String(const String& source)
            : id_{source.id_}
            , value_{source.value_}
        {
            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(cc: " << id_ << ", " << value_ << ")" << std::endl;
            #endif
            ++copy_constructor_count;
        }

        String& operator=(const String& source)
        {
            if (this != &source)
            {
                id_ = source.id_;
                value_ = source.value_;
            }

            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(c=: " << id_ << ", " << value_ << ")" << std::endl;
            #endif

            ++copy_assignment_count;

            return *this;
        }

#ifdef ENABLE_MOVE_SEMANTICS

        String(String&& source) noexcept
            : id_{source.id_}
            , value_{std::move(source.value_)}
        {
            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(mv: " << id_ << ", " << value_ << ")" << std::endl;
            #endif
            ++move_constructor_count;
        }

        String& operator=(String&& source)
        {
            if (this != &source)
            {
                id_ = source.id_;
                value_ = std::move(source.value_);
            }

            #ifdef ENABLE_LOGGING_TO_CONSOLE
                std::cout << "String(m=: " << id_ << ", " << value_ << ")" << std::endl;
            #endif

            ++move_assignment_count;

            return *this;
        }

#endif

        uint64_t id() const
        {
            return id_;
        }

        const std::string& value() const
        {
            return value_;
        }
    };

    inline String operator+(const String& lhs, const String& rhs)
    {
        return String{lhs.value() + rhs.value()};
    }

    inline std::ostream& operator<<(std::ostream& out, const String& g)
    {
        out << "String{id: " << g.id() << ", name: " << g.value() << "}";
        return out;
    }

    struct Vector : std::vector<String>
    {
        using std::vector<String>::vector;

#ifndef ENABLE_MOVE_SEMANTICS
        ~Vector() = default;
#endif
    };
} // namespace Helpers

#endif