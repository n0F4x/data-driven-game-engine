#include <catch2/catch_test_macros.hpp>

import core.ecs;

TEST_CASE("core::ecs::Registry")
{
    using Registry = core::ecs::Registry<>;
    Registry registry;

    SECTION("create")
    {
        decltype(auto) id = registry.create(int{}, float{});

        STATIC_REQUIRE(std::is_same_v<decltype(id), core::ecs::ID<Registry>>);
    }

    SECTION("get")
    {
        constexpr static int   integer{ 1 };
        constexpr static float floating{ 2 };

        const auto id = registry.create(integer, floating);

        decltype(auto) integer_tuple{ registry.get<int>(id) };
        STATIC_REQUIRE(std::is_same_v<decltype(integer_tuple), std::tuple<int&>>);
        REQUIRE(std::get<int&>(integer_tuple) == integer);

        decltype(auto) floating_tuple{ registry.get<float>(id) };
        STATIC_REQUIRE(std::is_same_v<decltype(floating_tuple), std::tuple<float&>>);
        REQUIRE(std::get<float&>(floating_tuple) == floating);

        decltype(auto) combined_tuple{ registry.get<int, float>(id) };
        STATIC_REQUIRE(std::is_same_v<decltype(combined_tuple), std::tuple<int&, float&>>);
        REQUIRE(combined_tuple == std::make_tuple(integer, floating));

        decltype(auto) shuffled_tuple{ registry.get<float, int>(id) };
        STATIC_REQUIRE(std::is_same_v<decltype(shuffled_tuple), std::tuple<float&, int&>>);
        REQUIRE(shuffled_tuple == std::make_tuple(floating, integer));
    }

    SECTION("destroy contained")
    {
        const auto id      = registry.create(int{}, float{});
        const bool success = registry.destroy(id);

        REQUIRE(success);
    }

    SECTION("destroy non-contained")
    {
        const bool success = registry.destroy(core::ecs::ID<Registry>{});

        REQUIRE(!success);
    }
}
