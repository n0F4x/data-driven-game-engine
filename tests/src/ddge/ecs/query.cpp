#include <catch2/catch_test_macros.hpp>

import ddge.ecs;

import ddge.util.containers.OptionalRef;
import ddge.util.containers.Tuple;
import ddge.util.meta.concepts.specialization_of;
import ddge.util.meta.type_traits.type_list.type_list_filter;
import ddge.util.meta.type_traits.type_list.type_list_transform;
import ddge.util.meta.type_traits.underlying;
import ddge.util.TypeList;

namespace ddge::ecs {

namespace {

template <query_filter_c T>
struct IsQueried {
    constexpr static bool value = queryable_component_c<std::remove_const_t<T>>
                               || std::same_as<T, ID>
                               || ddge::util::meta::specialization_of_c<T, Optional>;
};

template <query_filter_c T>
struct ToFunctionParameter;

template <query_filter_c T>
    requires queryable_component_c<std::remove_const_t<T>>
struct ToFunctionParameter<T> : std::type_identity<std::add_lvalue_reference_t<T>> {};

template <query_filter_c T>
    requires std::same_as<T, ID>
struct ToFunctionParameter<T> : std::type_identity<T> {};

template <query_filter_c T>
    requires ddge::util::meta::specialization_of_c<T, Optional>
struct ToFunctionParameter<T>
    : std::type_identity<ddge::util::OptionalRef<ddge::util::meta::underlying_t<T>>> {};

}   // namespace

TEST_CASE("query")
{
    Registry registry;

    SECTION("empty component")
    {
        constexpr static auto check = []<typename EmptyComponent_T>(
                                          std::type_identity<EmptyComponent_T>
                                      ) static -> bool {
            return not requires { Query<int, EmptyComponent_T>{ registry }; };
        };

        struct Empty {};

        static_assert(check(std::type_identity<Empty>{}));
    }

    SECTION("each (without id)")
    {
        SECTION("empty registry")
        {
            query<int>(registry, [](int&) {});
        }

        registry.create(int{}, float{});

        SECTION("single archetype")
        {
            {
                std::size_t visit_count{};

                query<int, float>(registry, [&](int&, float&) { ++visit_count; });
                query<int, float>(registry, [&](int&, float&) {});

                REQUIRE(visit_count == 1);
            }

            {
                std::size_t visit_count{};

                query<int>(registry, [&](int&) { ++visit_count; });
                query<int>(registry, +[](int&) {});

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{});

            {
                std::size_t visit_count{};

                query<int, float>(registry, [&](int&, float&) { ++visit_count; });

                REQUIRE(visit_count == 2);
            }
        }

        SECTION("multiple archetypes")
        {
            registry.create(int{}, float{}, long{});

            {
                std::size_t visit_count{};

                query<int, float>(registry, [&](int&, float&) { ++visit_count; });

                REQUIRE(visit_count == 2);
            }

            {
                std::size_t visit_count{};

                query<int, float, long>(registry, [&](int&, float&, long&) {
                    ++visit_count;
                });

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{}, long{});

            {
                std::size_t visit_count{};

                query<int, float, long>(registry, [&](int&, float&, long&) {
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
            query<ID, int>(registry, [&](const ID, int&) {});
        }

        registry.create(int{}, float{});

        const auto make_visitor = [&registry]<component_c... Components_T>(
                                      std::size_t& visit_count
                                  ) {
            return [&registry, &visit_count](const ID id, Components_T&... components) {
                ++visit_count;
                const auto found_components = registry.find_all<Components_T...>(id);
                REQUIRE(found_components.has_value());
                REQUIRE(found_components == util::Tuple{ components... });
            };
        };

        SECTION("single archetype")
        {
            {
                std::size_t visit_count{};

                query<ID, int, float>(
                    registry, make_visitor.operator()<int, float>(visit_count)
                );
                query<ID, int, float>(registry, +[](ID, int&, float&) {});

                REQUIRE(visit_count == 1);
            }

            {
                std::size_t visit_count{};

                query<ID, int>(registry, make_visitor.operator()<int>(visit_count));
                query<ID, int>(registry, +[](ID, int&) {});

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{});

            {
                std::size_t visit_count{};

                query<ID, int, float>(
                    registry, make_visitor.operator()<int, float>(visit_count)
                );

                REQUIRE(visit_count == 2);
            }
        }

        SECTION("multiple archetypes")
        {
            registry.create(int{}, float{}, long{});

            {
                std::size_t visit_count{};

                query<ID, int, float>(
                    registry, make_visitor.operator()<int, float>(visit_count)
                );

                REQUIRE(visit_count == 2);
            }

            {
                std::size_t visit_count{};

                query<ID, int, float, long>(
                    registry, make_visitor.operator()<int, float, long>(visit_count)
                );

                REQUIRE(visit_count == 1);
            }

            registry.create(int{}, float{}, long{});

            {
                std::size_t visit_count{};

                query<ID, int, float, long>(
                    registry, make_visitor.operator()<int, float, long>(visit_count)
                );

                REQUIRE(visit_count == 2);
            }
        }
    }

    SECTION("query parameter tags")
    {
        using namespace query_filter_tags;

        // Test that it compiles
        query<
            ID,
            float,
            const double,
            Without<int8_t>,
            With<int16_t>,
            With<int32_t>,
            Optional<uint8_t>,
            Optional<const uint16_t>>(
            registry,
            [](ID,
               float&,
               const double&,
               ddge::util::OptionalRef<uint8_t>,
               ddge::util::OptionalRef<const uint16_t>) {}
        );

        const auto test_query_parameters =
            [&registry]<typename... Ts, typename F = void (*)()>(
                ddge::util::TypeList<Ts...>, F func = +[] {}
            ) {
                using QueryFunctionParameters = ddge::util::meta::type_list_transform_t<
                    ddge::util::meta::
                        type_list_filter_t<ddge::util::TypeList<Ts...>, IsQueried>,
                    ToFunctionParameter>;

                [&registry, &func]<typename... Us>(ddge::util::TypeList<Us...>) {
                    query<Ts...>(registry, [&func](Us...) { std::invoke(func); });
                }(QueryFunctionParameters{});
            };

        SECTION("empty registry")
        {
            test_query_parameters(ddge::util::TypeList<float, ID>{});

            test_query_parameters(ddge::util::TypeList<float, Without<double>>{});

            test_query_parameters(ddge::util::TypeList<float, With<double>>{});

            test_query_parameters(ddge::util::TypeList<float, Optional<double>>{});

            test_query_parameters(ddge::util::TypeList<float, Optional<const double>>{});

            test_query_parameters(
                ddge::util::TypeList<
                    ID,
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

            std::size_t visit_count{};

            test_query_parameters(
                ddge::util::TypeList<
                    ID,
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

            REQUIRE(visit_count == 1);
        }

        SECTION("no actually queried component, but id")
        {
            registry.create(float{}, int16_t{}, int32_t{});
            registry.create(float{}, int8_t{}, int16_t{}, int32_t{});
            registry.create(float{}, int16_t{});
            registry.create(float{}, int16_t{}, int32_t{}, uint8_t{});

            std::size_t visit_count{};

            test_query_parameters(
                ddge::util::TypeList<ID, Without<int8_t>, With<int16_t>, With<int32_t>>{},
                [&visit_count] { ++visit_count; }
            );

            REQUIRE(visit_count == 2);

            static_assert(requires {
                !requires { query<ID, Without<int8_t>>(registry, [] {}); };
            });
        }

        SECTION("no actually queried component nor id")
        {
            static_assert(requires {
                !requires { query<Without<int8_t>>(registry, [] {}); };
            });
        }

        SECTION("deducing parameters")
        {
            registry.create(float{}, double{}, int16_t{}, int32_t{}, uint8_t{});
            registry.create(float{}, double{}, int8_t{}, int16_t{}, int32_t{}, uint8_t{});
            registry.create(float{}, double{}, int16_t{}, uint8_t{});
            registry.create(float{}, int16_t{}, int32_t{}, uint8_t{}, uint16_t{});

            std::size_t visit_count{};

            query(
                registry,
                [&visit_count](
                    ID,                  //
                    float&,              //
                    const double&,       //
                    Without<int8_t>,     //
                    With<int16_t>,       //
                    With<int32_t>,       //
                    Optional<uint8_t>,   //
                    Optional<const uint16_t>
                ) { visit_count++; }
            );

            REQUIRE(visit_count == 1);
        }
    }

    SECTION("empty query")
    {
        registry.create(int{});

        size_t count{};
        count = Query<With<int>>{ registry }.count();
        REQUIRE(count == 1);

        count = 0;
        Query<With<int>>{ registry }([&count] -> void { count++; });
        REQUIRE(count == 1);

        count = 0;
        query(registry, [&count](With<int>) -> void { count++; });
        REQUIRE(count == 1);
    }
}

TEST_CASE("count")
{
    Registry registry;

    registry.create(int{});
    registry.create(int{}, float{});

    REQUIRE((count<int>(registry) == 2));
}

}   // namespace ddge::ecs
