#include <catch2/catch_test_macros.hpp>

// TODO: remove these includes
#include <algorithm>
#include <ranges>

import core.ecs;

import utility.containers.OptionalRef;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;
import utility.TypeList;

namespace {

template <core::ecs::query_parameter_c T>
struct IsQueried {
    constexpr static bool value = core::ecs::queryable_component_c<std::remove_const_t<T>>
                               || std::same_as<T, core::ecs::ID>
                               || util::meta::specialization_of_c<T, core::ecs::Optional>;
};

template <core::ecs::query_parameter_c T>
struct ToFunctionParameter;

template <core::ecs::query_parameter_c T>
    requires core::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToFunctionParameter<T> : std::type_identity<std::add_lvalue_reference_t<T>> {};

template <core::ecs::query_parameter_c T>
    requires std::same_as<T, core::ecs::ID>
struct ToFunctionParameter<T> : std::type_identity<T> {};

template <core::ecs::query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::Optional>
struct ToFunctionParameter<T>
    : std::type_identity<util::OptionalRef<util::meta::underlying_t<T>>> {};

}   // namespace

TEST_CASE("core::ecs::query")
{
    core::ecs::Registry registry;

    SECTION("each (without id)")
    {
        SECTION("empty registry")
        {
            core::ecs::query<int>(registry, [&](int&) {});
        }

        registry.create(int{}, float{});

        SECTION("single archetype")
        {
            {
                size_t visit_count{};

                core::ecs::query<int, float>(registry, [&](int&, float&) {
                    ++visit_count;
                });
                core::ecs::query<int, float>(registry, [&](int&, float&) {});

                REQUIRE(visit_count == 1);
            }

            {
                size_t visit_count{};

                core::ecs::query<int>(registry, [&](int&) { ++visit_count; });
                core::ecs::query<int>(registry, +[](int&) {});

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{});

            {
                size_t visit_count{};

                core::ecs::query<int, float>(registry, [&](int&, float&) {
                    ++visit_count;
                });

                REQUIRE(visit_count == 2);
            }
        }

        SECTION("multiple archetypes")
        {
            registry.create(int{}, float{}, long{});

            {
                size_t visit_count{};

                core::ecs::query<int, float>(registry, [&](int&, float&) {
                    ++visit_count;
                });

                REQUIRE(visit_count == 2);
            }

            {
                size_t visit_count{};

                core::ecs::query<int, float, long>(registry, [&](int&, float&, long&) {
                    ++visit_count;
                });

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{}, long{});

            {
                size_t visit_count{};

                core::ecs::query<int, float, long>(registry, [&](int&, float&, long&) {
                    ++visit_count;
                });

                REQUIRE(visit_count == 2);
            }
        }
    }

    SECTION("each (with id)")
    {
        SECTION("empty registry")
        {
            core::ecs::query<core::ecs::ID, int>(
                registry, [&](const core::ecs::ID, int&) {}
            );
        }

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
                size_t visit_count{};

                core::ecs::query<core::ecs::ID, int, float>(
                    registry, make_visitor.operator()<int, float>(visit_count)
                );
                core::ecs::query<core::ecs::ID, int, float>(
                    registry, +[](core::ecs::ID, int&, float&) {}
                );

                REQUIRE(visit_count == 1);
            }

            {
                size_t visit_count{};

                core::ecs::query<core::ecs::ID, int>(
                    registry, make_visitor.operator()<int>(visit_count)
                );
                core::ecs::query<core::ecs::ID, int>(
                    registry, +[](core::ecs::ID, int&) {}
                );

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{});

            {
                size_t visit_count{};

                core::ecs::query<core::ecs::ID, int, float>(
                    registry, make_visitor.operator()<int, float>(visit_count)
                );

                REQUIRE(visit_count == 2);
            }
        }

        SECTION("multiple archetypes")
        {
            registry.create(int{}, float{}, long{});

            {
                size_t visit_count{};

                core::ecs::query<core::ecs::ID, int, float>(
                    registry, make_visitor.operator()<int, float>(visit_count)
                );

                REQUIRE(visit_count == 2);
            }

            {
                size_t visit_count{};

                core::ecs::query<core::ecs::ID, int, float, long>(
                    registry, make_visitor.operator()<int, float, long>(visit_count)
                );

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{}, long{});

            {
                size_t visit_count{};

                core::ecs::query<core::ecs::ID, int, float, long>(
                    registry, make_visitor.operator()<int, float, long>(visit_count)
                );

                REQUIRE(visit_count == 2);
            }
        }
    }

    SECTION("query parameter tags")
    {
        using namespace core::ecs::query_parameter_tags;

        const auto test_query_parameters =
            [&registry]<typename... Ts, typename F = void(*)()>(
                util::TypeList<Ts...>, F func = +[] {}
            ) {
                using QueryFunctionParameters = util::meta::type_list_transform_t<
                    util::meta::type_list_filter_t<util::TypeList<Ts...>, IsQueried>,
                    ToFunctionParameter>;

                [&registry, &func]<typename... Us>(util::TypeList<Us...>) {
                    core::ecs::query<Ts...>(registry, [&func](Us...) {
                        std::invoke(func);
                    });
                }(QueryFunctionParameters{});
            };

        SECTION("empty registry")
        {
            test_query_parameters(util::TypeList<float, core::ecs::ID>{});

            test_query_parameters(util::TypeList<float, Without<double>>{});

            test_query_parameters(util::TypeList<float, With<double>>{});

            test_query_parameters(util::TypeList<float, Optional<double>>{});

            test_query_parameters(util::TypeList<float, Optional<const double>>{});

            test_query_parameters(
                util::TypeList<
                    core::ecs::ID,
                    int8_t,
                    const int16_t,
                    Without<uint8_t>,
                    With<uint16_t>,
                    Optional<uint32_t>,
                    Optional<const uint64_t>>{}
            );
        }

        SECTION("exact match")
        {
            registry.create(float{}, double{}, int16_t{}, int32_t{}, uint8_t{});
            registry.create(float{}, double{}, int8_t{}, int16_t{}, int32_t{}, uint8_t{});
            registry.create(float{}, double{}, int16_t{}, uint8_t{});
            registry.create(float{}, int16_t{}, int32_t{}, uint8_t{}, uint16_t{});

            size_t visit_count{};

            test_query_parameters(
                util::TypeList<
                    core::ecs::ID,
                    float,
                    const double,
                    Without<int8_t>,
                    With<int16_t>,
                    With<int32_t>,
                    Optional<uint8_t>,
                    Optional<const uint16_t>,
                    Optional<uint32_t>,
                    Optional<const uint64_t>>{},
                [&visit_count] { ++visit_count; }
            );

            core::ecs::query<
                core::ecs::ID,
                float,
                const double,
                Without<int8_t>,
                With<int16_t>,
                With<int32_t>,
                Optional<uint8_t>,
                Optional<const uint16_t>>(
                registry,
                [](core::ecs::ID,
                   float&,
                   const double&,
                   util::OptionalRef<uint8_t>,
                   util::OptionalRef<const uint16_t>) {}
            );

            REQUIRE(visit_count == 1);
        }
    }
}
