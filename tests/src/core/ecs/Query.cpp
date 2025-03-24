#include <catch2/catch_test_macros.hpp>

import core.ecs;

TEST_CASE("core::ecs::Query")
{
    core::ecs::Registry registry;

    SECTION("construct from registry")
    {
        core::ecs::Query{ registry };
    }

    SECTION("type requirements")
    {
        static_assert([]<typename... Ts>() {
            return !requires(core::ecs::Registry dummy_registry) {
                core::ecs::Query<Ts...>{ dummy_registry };
            };
        }.operator()<int, int>());

        struct Empty {};

        static_assert([]<typename... Ts>() {
            return !requires(core::ecs::Registry dummy_registry) {
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

    SECTION("each (with id)")
    {
        registry.create(int{}, float{});

        const auto make_visitor = [&registry]<core::ecs::component_c... Components_T>(
                                      size_t& visit_count
                                  ) {
            return [&registry,
                    &visit_count](const core::ecs::ID id, Components_T&... components) {
                ++visit_count;
                const auto found_components = registry.find_all<Components_T...>(id);
                REQUIRE(found_components.has_value());
                REQUIRE(found_components == std::make_tuple(components...));
            };
        };

        SECTION("single archetype")
        {
            {
                core::ecs::Query<int, float> query{ registry };

                size_t visit_count{};
                query.each(make_visitor.operator()<int, float>(visit_count));
                query.each(+[](core::ecs::ID, int&, float&) {});

                REQUIRE(visit_count == 1);
            }

            {
                core::ecs::Query<int> query{ registry };

                size_t visit_count{};
                query.each(make_visitor.operator()<int>(visit_count));
                query.each(+[](core::ecs::ID, int&) {});

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{});

            {
                core::ecs::Query<int, float> query{ registry };

                size_t visit_count{};
                query.each(make_visitor.operator()<int, float>(visit_count));

                REQUIRE(visit_count == 2);
            }
        }

        SECTION("multiple archetypes")
        {
            registry.create(int{}, float{}, long{});

            {
                core::ecs::Query<int, float> query{ registry };

                size_t visit_count{};
                query.each(make_visitor.operator()<int, float>(visit_count));

                REQUIRE(visit_count == 2);
            }

            {
                core::ecs::Query<int, float, long> query{ registry };

                size_t visit_count{};
                query.each(make_visitor.operator()<int, float, long>(visit_count));

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{}, long{});

            {
                core::ecs::Query<int, float, long> query{ registry };

                size_t visit_count{};
                query.each(make_visitor.operator()<int, float, long>(visit_count));

                REQUIRE(visit_count == 2);
            }
        }
    }
}
