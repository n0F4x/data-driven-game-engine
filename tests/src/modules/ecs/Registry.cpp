#include <catch2/catch_test_macros.hpp>

import ddge.modules.ecs;
import ddge.utility.containers.OptionalRef;
import ddge.utility.containers.StringLiteral;
import ddge.utility.contracts;
import ddge.utility.meta.algorithms.apply;
import ddge.utility.meta.algorithms.enumerate;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.type_traits.type_list.type_list_index_of;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.meta.type_traits.type_list.type_list_size;
import ddge.utility.meta.type_traits.const_like;
import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.meta.type_traits.underlying;
import ddge.utility.tuple.tuple_all_of;
import ddge.utility.tuple.tuple_any_of;
import ddge.utility.TypeList;

using RegistryValueCategories = ddge::util::TypeList<
    ddge::ecs::Registry&,
    const ddge::ecs::Registry&,
    ddge::ecs::Registry&&,
    const ddge::ecs::Registry&&>;

using RegularComponents = ddge::util::
    TypeList<float, double, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;

template <std::size_t>
struct Empty {
    constexpr explicit(false) Empty(int) {}

    constexpr auto operator<=>(const Empty&) const = default;
};

template <typename T>
struct ToEmpty {
    using type = Empty<ddge::util::meta::type_list_index_of_v<RegularComponents, T>>;
};

using EmptyComponents =
    ddge::util::meta::type_list_transform_t<RegularComponents, ToEmpty>;

using ComponentGroups = ddge::util::TypeList<RegularComponents, EmptyComponents>;

constexpr static std::
    array<ddge::util::StringLiteral, ddge::util::meta::type_list_size_v<ComponentGroups>>
        component_group_names{ "regular components", "empty components" };

using namespace std::literals;

TEST_CASE("ddge::ecs::Registry")
{
    ddge::ecs::Registry registry;

    ddge::util::meta::enumerate<
        ComponentGroups>([&registry]<std::size_t index_T, typename ComponentGroup_T> {
        ddge::util::meta::apply<ComponentGroup_T>([&registry]<typename... Comps> {
            SECTION(component_group_names[index_T].get())
            {
                SECTION("create")
                {
                    decltype(auto) id = registry.create(Comps...[0](1), Comps...[1](2));

                    static_assert(std::is_same_v<decltype(id), ddge::ecs::ID>);
                }

                SECTION("destroy")
                {
                    const auto id = registry.create(Comps...[0](1), Comps...[1](2));

                    const bool successfully_destroyed_contained = registry.destroy(id);
                    REQUIRE(successfully_destroyed_contained);

                    const bool successfully_destroyed_non_contained = registry.destroy(id);
                    REQUIRE_FALSE(successfully_destroyed_non_contained);
                }

                SECTION("get")
                {
                    ddge::util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{
                            ddge::util::meta::name_of<Registry_T>()
                        };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0 = 1;
                            constexpr static std::type_identity_t<Comps...[1]> comp_1 = 2;

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) empty{
                                static_cast<Registry_T>(registry).template get<>(id)
                            };
                            static_assert(std::is_same_v<decltype(empty), std::tuple<>>);

                            decltype(auto) comp_0_tuple{
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[0]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(comp_0_tuple),
                                          std::tuple<ddge::util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(
                                (std::get<ddge::util::meta::const_like_t<
                                     Comps...[0],
                                     std::remove_reference_t<Registry_T>>&>(comp_0_tuple)
                                 == comp_0)
                            );

                            decltype(auto) comp_1_tuple{
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[1]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(comp_1_tuple),
                                          std::tuple<ddge::util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(
                                (std::get<ddge::util::meta::const_like_t<
                                     Comps...[1],
                                     std::remove_reference_t<Registry_T>>&>(comp_1_tuple)
                                 == comp_1)
                            );

                            decltype(auto) combined_tuple{
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[0], Comps...[1]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(combined_tuple),
                                          std::tuple<
                                              ddge::util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>&,
                                              ddge::util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(combined_tuple == std::make_tuple(comp_0, comp_1));

                            decltype(auto) shuffled_tuple{
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[1], Comps...[0]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(shuffled_tuple),
                                          std::tuple<
                                              ddge::util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>&,
                                              ddge::util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(shuffled_tuple == std::make_tuple(comp_1, comp_0));

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry).template get<Comps...[0]>(
                                    ddge::ecs::Registry::null_id
                                ),
                                ddge::util::PreconditionViolation
                            );

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[2]>(id),
                                ddge::util::PreconditionViolation
                            );

                            REQUIRE_THROWS_AS(
                                (static_cast<Registry_T>(registry)
                                     .template get<Comps...[0], Comps...[1], Comps...[2]>(
                                         id
                                     )),
                                ddge::util::PreconditionViolation
                            );

                            static_assert(!requires {
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[0], Comps...[0]>(id);
                            });
                        }
                    });
                }

                SECTION("get_single")
                {
                    ddge::util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{
                            ddge::util::meta::name_of<Registry_T>()
                        };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) comp_0_result{
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[0]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(comp_0_result),
                                          ddge::util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>&>);
                            REQUIRE(comp_0_result == comp_0);

                            decltype(auto) comp_1_result{
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[1]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(comp_1_result),
                                          ddge::util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>&>);
                            REQUIRE(comp_1_result == comp_1);

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[0]>(
                                        ddge::ecs::Registry::null_id
                                    ),
                                ddge::util::PreconditionViolation
                            );

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[2]>(id),
                                ddge::util::PreconditionViolation
                            );

                            static_assert(!requires {
                                static_cast<Registry_T>(registry).template get_single<>(
                                    id
                                );
                            });

                            static_assert(!requires {
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[0], Comps...[1]>(id);
                            });
                        }
                    });
                }

                SECTION("find")
                {
                    ddge::util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{
                            ddge::util::meta::name_of<Registry_T>()
                        };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) empty{
                                static_cast<Registry_T>(registry).template find<>(id)
                            };
                            static_assert(std::is_same_v<decltype(empty), std::tuple<>>);

                            decltype(auto) optional_comp_0_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find<Comps...[0]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(optional_comp_0_tuple),
                                          std::tuple<ddge::util::OptionalRef<
                                              ddge::util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>>>>);
                            REQUIRE(
                                ddge::util::tuple_all_of(
                                    optional_comp_0_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE((
                                *std::get<
                                    ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                        Comps...[0],
                                        std::remove_reference_t<Registry_T>>>>(
                                    optional_comp_0_tuple
                                )
                                == comp_0
                            ));

                            decltype(auto) optional_comp_1_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find<Comps...[1]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(optional_comp_1_tuple),
                                          std::tuple<ddge::util::OptionalRef<
                                              ddge::util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>>>>);
                            REQUIRE(
                                ddge::util::tuple_all_of(
                                    optional_comp_1_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE((
                                *std::get<
                                    ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                        Comps...[1],
                                        std::remove_reference_t<Registry_T>>>>(
                                    optional_comp_1_tuple
                                )
                                == comp_1
                            ));

                            decltype(auto) optional_combined_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find<Comps...[0], Comps...[1]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(optional_combined_tuple),
                                    std::tuple<
                                        ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                            Comps...[0],
                                            std::remove_reference_t<Registry_T>>>,
                                        ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                            Comps...[1],
                                            std::remove_reference_t<Registry_T>>>>>
                            );
                            REQUIRE(
                                ddge::util::tuple_all_of(
                                    optional_combined_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE(
                                ddge::util::tuple_all_of(
                                    optional_combined_tuple,
                                    []<typename T>(T value) static {
                                        if constexpr (std::is_same_v<
                                                          std::remove_cvref_t<T>,
                                                          Comps...[0]>)
                                        {
                                            return value == comp_0;
                                        }
                                        else if constexpr (std::is_same_v<
                                                               std::remove_cvref_t<T>,
                                                               Comps...[1]>)
                                        {
                                            return value == comp_1;
                                        }
                                    },
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return *optional;
                                    }
                                )
                            );

                            decltype(auto) optional_shuffled_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find<Comps...[1], Comps...[0]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(optional_shuffled_tuple),
                                    std::tuple<
                                        ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                            Comps...[1],
                                            std::remove_reference_t<Registry_T>>>,
                                        ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                            Comps...[0],
                                            std::remove_reference_t<Registry_T>>>>>
                            );
                            REQUIRE(
                                ddge::util::tuple_all_of(
                                    optional_shuffled_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE(
                                ddge::util::tuple_all_of(
                                    optional_shuffled_tuple,
                                    []<typename T>(T value) static {
                                        if constexpr (std::is_same_v<
                                                          std::remove_cvref_t<T>,
                                                          Comps...[0]>)
                                        {
                                            return value == comp_0;
                                        }
                                        else if constexpr (std::is_same_v<
                                                               std::remove_cvref_t<T>,
                                                               Comps...[1]>)
                                        {
                                            return value == comp_1;
                                        }
                                    },
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return *optional;
                                    }
                                )
                            );

                            REQUIRE_FALSE(
                                ddge::util::tuple_any_of(
                                    static_cast<Registry_T>(registry)
                                        .template find<Comps...[0]>(
                                            ddge::ecs::Registry::null_id
                                        ),
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );

                            REQUIRE_FALSE(
                                ddge::util::tuple_any_of(
                                    static_cast<Registry_T>(registry)
                                        .template find<Comps...[2]>(id),
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );

                            static_assert(!requires {
                                static_cast<Registry_T>(registry)
                                    .template find<Comps...[0], Comps...[0]>(id);
                            });
                        }
                    });
                }

                SECTION("find_all")
                {
                    ddge::util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{
                            ddge::util::meta::name_of<Registry_T>()
                        };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) empty{
                                static_cast<Registry_T>(registry).template find_all<>(id)
                            };
                            static_assert(
                                std::is_same_v<decltype(empty), std::optional<std::tuple<>>>
                            );

                            decltype(auto) optional_comp_0_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(optional_comp_0_tuple),
                                    std::optional<std::tuple<ddge::util::meta::const_like_t<
                                        Comps...[0],
                                        std::remove_reference_t<Registry_T>>&>>>
                            );
                            REQUIRE(optional_comp_0_tuple.has_value());
                            REQUIRE(
                                (std::get<ddge::util::meta::const_like_t<
                                     Comps...[0],
                                     std::remove_reference_t<Registry_T>>&>(
                                     optional_comp_0_tuple.value()
                                 )
                                 == comp_0)
                            );

                            decltype(auto) optional_comp_1_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[1]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(optional_comp_1_tuple),
                                    std::optional<std::tuple<ddge::util::meta::const_like_t<
                                        Comps...[1],
                                        std::remove_reference_t<Registry_T>>&>>>
                            );
                            REQUIRE(optional_comp_1_tuple.has_value());
                            REQUIRE(
                                (std::get<ddge::util::meta::const_like_t<
                                     Comps...[1],
                                     std::remove_reference_t<Registry_T>>&>(
                                     optional_comp_1_tuple.value()
                                 )
                                 == comp_1)
                            );

                            decltype(auto) optional_combined_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0], Comps...[1]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(optional_combined_tuple),
                                    std::optional<std::tuple<
                                        ddge::util::meta::const_like_t<
                                            Comps...[0],
                                            std::remove_reference_t<Registry_T>>&,
                                        ddge::util::meta::const_like_t<
                                            Comps...[1],
                                            std::remove_reference_t<Registry_T>>&>>>
                            );
                            REQUIRE(optional_combined_tuple.has_value());
                            REQUIRE(
                                optional_combined_tuple.value()
                                == std::make_tuple(comp_0, comp_1)
                            );

                            decltype(auto) optional_shuffled_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[1], Comps...[0]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(optional_shuffled_tuple),
                                    std::optional<std::tuple<
                                        ddge::util::meta::const_like_t<
                                            Comps...[1],
                                            std::remove_reference_t<Registry_T>>&,
                                        ddge::util::meta::const_like_t<
                                            Comps...[0],
                                            std::remove_reference_t<Registry_T>>&>>>
                            );
                            REQUIRE(optional_shuffled_tuple.has_value());
                            REQUIRE(
                                optional_shuffled_tuple.value()
                                == std::make_tuple(comp_1, comp_0)
                            );

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0]>(
                                        ddge::ecs::Registry::null_id
                                    )
                                    .has_value()
                            );

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[2]>(id)
                                    .has_value()
                            );

                            REQUIRE_FALSE((
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0], Comps...[1], Comps...[2]>(
                                        id
                                    )
                                    .has_value()
                            ));

                            static_assert(!requires {
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0], Comps...[0]>(id);
                            });
                        }
                    });
                }

                SECTION("find_single")
                {
                    ddge::util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{
                            ddge::util::meta::name_of<Registry_T>()
                        };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) comp_0_result{
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[0]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(comp_0_result),
                                    ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                        Comps...[0],
                                        std::remove_reference_t<Registry_T>>>>
                            );
                            REQUIRE(comp_0_result.has_value());
                            REQUIRE(comp_0_result.has_value());
                            REQUIRE(*comp_0_result == comp_0);

                            decltype(auto) comp_1_result{
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[1]>(id)
                            };
                            static_assert(
                                std::is_same_v<
                                    decltype(comp_1_result),
                                    ddge::util::OptionalRef<ddge::util::meta::const_like_t<
                                        Comps...[1],
                                        std::remove_reference_t<Registry_T>>>>
                            );
                            REQUIRE(comp_1_result.has_value());
                            REQUIRE(*comp_1_result == comp_1);

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[0]>(
                                        ddge::ecs::Registry::null_id
                                    )
                                    .has_value()
                            );

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[2]>(id)
                                    .has_value()
                            );

                            static_assert(!requires {
                                static_cast<Registry_T>(registry).template find_single<>(
                                    id
                                );
                            });

                            static_assert(!requires {
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[0], Comps...[1]>(id);
                            });
                        }
                    });
                }

                SECTION("contains_all")
                {
                    const auto id = registry.create(Comps...[0](1), Comps...[1](2));

                    REQUIRE(registry.contains_all<>(id));
                    REQUIRE(registry.contains_all<Comps...[0]>(id));
                    REQUIRE(registry.contains_all<Comps...[1]>(id));
                    REQUIRE(registry.contains_all<Comps...[0], Comps...[1]>(id));
                    REQUIRE(registry.contains_all<Comps...[1], Comps...[0]>(id));

                    REQUIRE_FALSE(
                        registry.contains_all<Comps...[1], Comps...[0], Comps...[2]>(id)
                    );
                    REQUIRE_FALSE(registry.contains_all<Comps...[2]>(id));
                    REQUIRE_FALSE(registry.contains_all<>(ddge::ecs::Registry::null_id));

                    []<typename... Components_T>() {
                        static_assert(!requires(
                            ddge::ecs::Registry registry, ddge::ecs::ID id
                        ) { registry.contains_all<Components_T...>(id); });
                    }.template operator()<Comps...[0], Comps...[0]>();
                }

                SECTION("insert")
                {
                    const auto id = registry.create(Comps...[0](1), Comps...[1](2));

                    registry.insert(id);

                    registry.insert(id, Comps...[2](3), Comps...[3](4));
                    REQUIRE(
                        registry
                            .contains_all<Comps...[0], Comps...[1], Comps...[2], Comps...[3]>(
                                id
                            )
                    );

                    REQUIRE_THROWS_AS(
                        registry.insert(ddge::ecs::Registry::null_id, Comps...[0](1)),
                        ddge::util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.insert(
                            id,
                            Comps...[2](3),
                            Comps...[3](4),
                            Comps...[4](5),
                            Comps...[5](6)
                        ),
                        ddge::util::PreconditionViolation
                    );
                }

                SECTION("insert_or_replace")
                {
                    const auto id = registry.create(Comps...[0](1), Comps...[1](2));

                    registry.insert_or_replace(id);

                    registry.insert_or_replace(id, Comps...[2](3), Comps...[3](4));
                    REQUIRE(
                        registry
                            .contains_all<Comps...[0], Comps...[1], Comps...[2], Comps...[3]>(
                                id
                            )
                    );

                    registry.insert_or_replace(
                        id, Comps...[2](3), Comps...[3](4), Comps...[4](5), Comps...[5](6)
                    );
                    REQUIRE(   //
                        registry.contains_all<
                            Comps...[0],
                            Comps...[1],
                            Comps...[2],
                            Comps...[3],
                            Comps...[4],
                            Comps...[5]>(id)
                    );

                    REQUIRE_THROWS_AS(
                        registry.insert_or_replace(
                            ddge::ecs::Registry::null_id, Comps...[0](1)
                        ),
                        ddge::util::PreconditionViolation
                    );
                }

                SECTION("remove")
                {
                    const auto id = registry.create(
                        Comps...[0](1),
                        Comps...[1](2),
                        Comps...[2](3),
                        Comps...[3](4),
                        Comps...[4](5),
                        Comps...[5](6)
                    );

                    decltype(auto) result_0 = registry.remove(id);
                    static_assert(std::is_same_v<decltype(result_0), std::tuple<>>);

                    decltype(auto) result_1 =
                        registry.remove<Comps...[4], Comps...[5]>(id);
                    static_assert(std::is_same_v<
                                  decltype(result_1),
                                  std::tuple<Comps...[4], Comps...[5]>>);
                    REQUIRE(
                        registry
                            .contains_all<Comps...[0], Comps...[1], Comps...[2], Comps...[3]>(
                                id
                            )
                    );
                    REQUIRE_FALSE(registry.contains_all<Comps...[4]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[5]>(id));

                    REQUIRE_THROWS_AS(
                        registry.remove<Comps...[0]>(ddge::ecs::Registry::null_id),
                        ddge::util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.remove<Comps...[4]>(id), ddge::util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.remove<Comps...[5]>(id), ddge::util::PreconditionViolation
                    );
                }

                SECTION("remove_single")
                {
                    const auto id = registry.create(
                        Comps...[0](1),
                        Comps...[1](2),
                        Comps...[2](3),
                        Comps...[3](4),
                        Comps...[4](5),
                        Comps...[5](6)
                    );

                    decltype(auto) result = registry.remove_single<Comps...[5]>(id);
                    static_assert(std::is_same_v<decltype(result), Comps...[5]>);
                    REQUIRE(registry.contains_all<
                            Comps...[0],
                            Comps...[1],
                            Comps...[2],
                            Comps...[3],
                            Comps...[4]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[5]>(id));

                    REQUIRE_THROWS_AS(
                        registry.remove_single<Comps...[0]>(ddge::ecs::Registry::null_id),
                        ddge::util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.remove_single<Comps...[5]>(id),
                        ddge::util::PreconditionViolation
                    );
                }

                SECTION("erase")
                {
                    const auto id = registry.create(
                        Comps...[0](1),
                        Comps...[1](2),
                        Comps...[2](3),
                        Comps...[3](4),
                        Comps...[4](5),
                        Comps...[5](6)
                    );

                    decltype(auto) result_0 = registry.erase(id);
                    static_assert(std::is_same_v<decltype(result_0), std::tuple<>>);

                    decltype(auto) result_1 = registry.erase<Comps...[5], Comps...[6]>(id);
                    static_assert(
                        std::is_same_v<
                            decltype(result_1),
                            std::tuple<std::optional<Comps...[5]>, std::optional<Comps...[6]>>>
                    );
                    REQUIRE(std::get<std::optional<Comps...[5]>>(result_1).has_value());
                    REQUIRE_FALSE(
                        std::get<std::optional<Comps...[6]>>(result_1).has_value()
                    );
                    REQUIRE(registry.contains_all<
                            Comps...[0],
                            Comps...[1],
                            Comps...[2],
                            Comps...[3],
                            Comps...[4]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[5]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[6]>(id));

                    REQUIRE_FALSE(
                        std::get<std::optional<Comps...[0]>>(
                            registry.erase<Comps...[0]>(ddge::ecs::Registry::null_id)
                        )
                            .has_value()
                    );
                    REQUIRE_FALSE(
                        std::get<std::optional<Comps...[5]>>(
                            registry.erase<Comps...[5]>(id)
                        )
                            .has_value()
                    );
                }

                SECTION("erase_all")
                {
                    const auto id = registry.create(
                        Comps...[0](1),
                        Comps...[1](2),
                        Comps...[2](3),
                        Comps...[3](4),
                        Comps...[4](5),
                        Comps...[5](6)
                    );

                    decltype(auto) result_0 = registry.erase_all(id);
                    static_assert(
                        std::is_same_v<decltype(result_0), std::optional<std::tuple<>>>
                    );
                    REQUIRE(result_0.has_value());

                    decltype(auto) result_1 =
                        registry.erase_all<Comps...[4], Comps...[5]>(id);
                    static_assert(std::is_same_v<
                                  decltype(result_1),
                                  std::optional<std::tuple<Comps...[4], Comps...[5]>>>);
                    REQUIRE(result_1.has_value());
                    REQUIRE(
                        registry
                            .contains_all<Comps...[0], Comps...[1], Comps...[2], Comps...[3]>(
                                id
                            )
                    );
                    REQUIRE_FALSE(registry.contains_all<Comps...[4]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[5]>(id));

                    REQUIRE_FALSE(
                        registry.erase_all<Comps...[0]>(ddge::ecs::Registry::null_id)
                            .has_value()
                    );
                    REQUIRE_FALSE(registry.erase_all<Comps...[4]>(id).has_value());
                    REQUIRE_FALSE(registry.erase_all<Comps...[5]>(id).has_value());
                }

                SECTION("erase_single")
                {
                    const auto id = registry.create(
                        Comps...[0](1),
                        Comps...[1](2),
                        Comps...[2](3),
                        Comps...[3](4),
                        Comps...[4](5),
                        Comps...[5](6)
                    );

                    decltype(auto) result = registry.erase_single<Comps...[5]>(id);
                    static_assert(
                        std::is_same_v<decltype(result), std::optional<Comps...[5]>>
                    );
                    REQUIRE(result.has_value());
                    REQUIRE(registry.contains_all<
                            Comps...[0],
                            Comps...[1],
                            Comps...[2],
                            Comps...[3],
                            Comps...[4]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[5]>(id));

                    REQUIRE_FALSE(
                        registry.erase_single<Comps...[0]>(ddge::ecs::Registry::null_id)
                            .has_value()
                    );
                    REQUIRE_FALSE(registry.erase_single<Comps...[5]>(id).has_value());
                }
            }
        });
    });
}
