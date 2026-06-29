#include <concepts>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

import ddge.utility.containers.Function;

namespace ddge::util {

namespace {

constexpr auto throwing_function() -> void {}

[[maybe_unused]]
constexpr auto noexcept_function() noexcept -> void
{}

struct Dummy {
    // ReSharper disable once CppMemberFunctionMayBeStatic
    auto method() -> void {}
};

}   // namespace

TEST_CASE("ddge::util::Function")
{
    STATIC_REQUIRE(std::has_single_bit(sizeof(Function<void()>)));
    STATIC_REQUIRE(std::has_single_bit(alignof(Function<void()>)));

    SECTION("construct from function")
    {
        Function<void()>{ throwing_function };
    }

    SECTION("construct from member-function-pointer")
    {
        Function<void(Dummy)>{ &Dummy::method };
    }

    SECTION("construct from lambda")
    {
        Function<void()>{ [] {} };
    }

    SECTION("construct from static lambda")
    {
        Function<void()>{ [] static {} };
    }

    SECTION("construct from throwing")
    {
        STATIC_REQUIRE(
            std::constructible_from<Function<void() noexcept>, decltype(noexcept_function)>
            && !std::
                   constructible_from<Function<void() noexcept>, decltype(throwing_function)>

        );
    }

    SECTION("non-ref call")
    {
        Function<void()> any_function{ throwing_function };

        any_function();
        std::move(any_function)();
    }

    SECTION("& call")
    {
        []<typename Function_T> -> void {
            STATIC_REQUIRE(requires(Function_T function) { function(); });
            STATIC_REQUIRE(not requires(Function_T function) { std::move(function)(); });
        }.operator()<Function<void() &>>();
    }

    SECTION("& call")
    {
        []<typename Function_T> -> void {
            STATIC_REQUIRE(not requires(Function_T function) { function(); });
            STATIC_REQUIRE(requires(Function_T function) { std::move(function)(); });
        }.operator()<Function<void() &&>>();
    }

    SECTION("noexcept call")
    {
        STATIC_REQUIRE(std::is_nothrow_invocable_v<Function<void() noexcept>>);
    }

    SECTION("throwing call")
    {
        STATIC_REQUIRE(!std::is_nothrow_invocable_v<Function<void()>>);
    }
}

}   // namespace ddge::util
