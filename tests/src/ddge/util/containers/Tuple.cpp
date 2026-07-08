#include <type_traits>

#include <catch2/catch_test_macros.hpp>

import ddge.util.containers.Tuple;

namespace ddge::util {

TEST_CASE("ddge::util::Tuple")
{
    SECTION("constructor")
    {
        Tuple{};
        Tuple<int>{ 42 };
        Tuple<int, int>{ 0, 1 };
    }

    SECTION("get<index_T>")
    {
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<0>(std::declval<Tuple<int>&>())), int&>
        );
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<0>(std::declval<Tuple<int>>())), int&&>
        );
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<0>(std::declval<const Tuple<int>&>())), const int&>
        );
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<0>(std::declval<const Tuple<int>>())), const int&&>
        );

        {
            constexpr static Tuple<int> tuple{};

            STATIC_REQUIRE(get<0>(tuple) == 0);
        }

        []<template <typename...> typename Tuple_T, typename T> -> void {
            STATIC_REQUIRE(not requires(Tuple_T<> tuple) { get<0>(tuple); });
            STATIC_REQUIRE(requires(Tuple_T<T, T> tuple) { get<0>(tuple); });
            STATIC_REQUIRE(requires(Tuple_T<T, T> tuple) { get<1>(tuple); });
        }.operator()<Tuple, int>();
    }

    SECTION("get<T>")
    {
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<int>(std::declval<Tuple<int>&>())), int&>
        );
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<int>(std::declval<Tuple<int>>())), int&&>
        );
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<int>(std::declval<const Tuple<int>&>())), const int&>
        );
        STATIC_REQUIRE(
            std::is_same_v<decltype(get<int>(std::declval<const Tuple<int>>())), const int&&>
        );

        {
            constexpr static Tuple<int> tuple{};

            STATIC_REQUIRE(get<int>(tuple) == 0);
        }

        []<typename T, typename U> -> void {
            STATIC_REQUIRE(not requires(Tuple<T> tuple) { get<U>(tuple); });
            STATIC_REQUIRE(not requires(Tuple<T, T> tuple) { get<T>(tuple); });
        }.operator()<int, float>();
    }
}

}   // namespace ddge::util
