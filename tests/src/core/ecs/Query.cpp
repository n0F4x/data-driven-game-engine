#include <map> // TODO: remove when clang allows it

#include <catch2/catch_test_macros.hpp>

import core.ecs;

TEST_CASE("core::ecs::Query")
{
    using Registry = core::ecs::Registry<>;
    Registry registry;

    SECTION("construct from registry")
    {
        core::ecs::Query{ registry };
    }

    SECTION("type requirements")
    {
        static_assert([]<typename... Ts>() {
            return !requires(Registry dummy_registry) {
                core::ecs::Query<Ts...>{ dummy_registry };
            };
        }.operator()<int, int>());

        struct Empty {};

        static_assert([]<typename... Ts>() {
            return !requires(Registry dummy_registry) {
                core::ecs::Query<Ts...>{ dummy_registry };
            };
        }.operator()<Empty>());
    }

    SECTION("each (without id)")
    {
        registry.create(int{}, float{});

        SECTION("single archetype")
        {
            {
                core::ecs::Query<int, float> query{ registry };

                size_t visit_count{};
                query.each([&](int&, float&) { ++visit_count; });
                query.each(+[](int&, float&) {});

                REQUIRE(visit_count == 1);
            }

            {
                core::ecs::Query<int> query{ registry };

                size_t visit_count{};
                query.each([&](int&) { ++visit_count; });
                query.each(+[](int&) {});

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{});

            {
                core::ecs::Query<int, float> query{ registry };

                size_t visit_count{};
                query.each([&](int&, float&) { ++visit_count; });

                REQUIRE(visit_count == 2);
            }

            static_assert([]<typename Query_T>() {
                return !requires(Query_T query) { query.each([] static {}); };
            }.operator()<core::ecs::Query<>>());
        }

        SECTION("multiple archetypes")
        {
            registry.create(int{}, float{}, long{});

            {
                core::ecs::Query<int, float> query{ registry };

                size_t visit_count{};
                query.each([&](int&, float&) { ++visit_count; });

                REQUIRE(visit_count == 2);
            }

            {
                core::ecs::Query<int, float, long> query{ registry };

                size_t visit_count{};
                query.each([&](int&, float&, long&) { ++visit_count; });

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{}, long{});

            {
                core::ecs::Query<int, float, long> query{ registry };

                size_t visit_count{};
                query.each([&](int&, float&, long&) { ++visit_count; });

                REQUIRE(visit_count == 2);
            }
        }
    }
}
