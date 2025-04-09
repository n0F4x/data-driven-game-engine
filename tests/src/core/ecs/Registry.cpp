#include <catch2/catch_test_macros.hpp>

#include "utility/contracts.hpp"

// TODO: remove this include once it compiles without it
#include <unordered_map>

import core.ecs;
import utility.containers.OptionalRef;
import utility.meta.algorithms.apply;
import utility.meta.algorithms.enumerate;
import utility.meta.algorithms.for_each;
import utility.meta.reflection.name_of;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.underlying;
import utility.tuple.tuple_all_of;
import utility.tuple.tuple_any_of;
import utility.TypeList;

using RegistryValueCategories = util::TypeList<
    core::ecs::Registry&,
    const core::ecs::Registry&,
    core::ecs::Registry&&,
    const core::ecs::Registry&&>;

using RegularComponents = util::
    TypeList<float, double, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;

template <size_t>
struct Empty {
    constexpr explicit(false) Empty(int) {}

    constexpr auto operator<=>(const Empty&) const = default;
};

template <typename T>
struct ToEmpty {
    using type = Empty<util::meta::type_list_index_of_v<RegularComponents, T>>;
};

using EmptyComponents = util::meta::type_list_transform_t<RegularComponents, ToEmpty>;

using ComponentGroups = util::TypeList<RegularComponents, EmptyComponents>;

constexpr static std::array<const char*, util::meta::type_list_size_v<ComponentGroups>>
    component_group_names{ "regular components", "empty components" };

using namespace std::literals;

TEST_CASE("core::ecs::Registry")
{
    core::ecs::Registry registry;

    util::meta::enumerate<
        ComponentGroups>([&registry]<size_t index_T, typename ComponentGroup_T> {
        util::meta::apply<ComponentGroup_T>([&registry]<typename... Comps> {
            SECTION(component_group_names[index_T])
            {
                SECTION("create")
                {
                    decltype(auto) id = registry.create(Comps...[0](1), Comps...[1](2));

                    static_assert(std::is_same_v<decltype(id), core::ecs::ID>);
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
                    util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{ util::meta::name_of<Registry_T>(
                        ) };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0 = 1;
                            constexpr static std::type_identity_t<Comps...[1]> comp_1 = 2;

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) empty{
                                static_cast<Registry_T>(registry).template get<>(id)
                            };
                            static_assert(std::is_same_v<decltype(empty), std::tuple<>>);

                            decltype(auto
                            ) comp_0_tuple{ static_cast<Registry_T>(registry)
                                                .template get<Comps...[0]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(comp_0_tuple),
                                          std::tuple<util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(
                                (std::get<util::meta::const_like_t<
                                     Comps...[0],
                                     std::remove_reference_t<Registry_T>>&>(comp_0_tuple)
                                 == comp_0)
                            );

                            decltype(auto
                            ) comp_1_tuple{ static_cast<Registry_T>(registry)
                                                .template get<Comps...[1]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(comp_1_tuple),
                                          std::tuple<util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(
                                (std::get<util::meta::const_like_t<
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
                                              util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>&,
                                              util::meta::const_like_t<
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
                                              util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>&,
                                              util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>&>>);
                            REQUIRE(shuffled_tuple == std::make_tuple(comp_1, comp_0));

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry).template get<Comps...[0]>(
                                    core::ecs::Registry::null_id
                                ),
                                util::PreconditionViolation
                            );

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry)
                                    .template get<Comps...[2]>(id),
                                util::PreconditionViolation
                            );

                            REQUIRE_THROWS_AS(
                                (static_cast<Registry_T>(registry)
                                     .template get<Comps...[0], Comps...[1], Comps...[2]>(id
                                     )),
                                util::PreconditionViolation
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
                    util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{ util::meta::name_of<Registry_T>(
                        ) };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto
                            ) comp_0_result{ static_cast<Registry_T>(registry)
                                                 .template get_single<Comps...[0]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(comp_0_result),
                                          util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>&>);
                            REQUIRE(comp_0_result == comp_0);

                            decltype(auto
                            ) comp_1_result{ static_cast<Registry_T>(registry)
                                                 .template get_single<Comps...[1]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(comp_1_result),
                                          util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>&>);
                            REQUIRE(comp_1_result == comp_1);

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[0]>(
                                        core::ecs::Registry::null_id
                                    ),
                                util::PreconditionViolation
                            );

                            REQUIRE_THROWS_AS(
                                static_cast<Registry_T>(registry)
                                    .template get_single<Comps...[2]>(id),
                                util::PreconditionViolation
                            );

                            static_assert(!requires {
                                static_cast<Registry_T>(registry).template get_single<>(id
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
                    util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{ util::meta::name_of<Registry_T>(
                        ) };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) empty{
                                static_cast<Registry_T>(registry).template find<>(id)
                            };
                            static_assert(std::is_same_v<decltype(empty), std::tuple<>>);

                            decltype(auto
                            ) optional_comp_0_tuple{ static_cast<Registry_T>(registry)
                                                         .template find<Comps...[0]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(optional_comp_0_tuple),
                                          std::tuple<util::OptionalRef<util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>>>>);
                            REQUIRE(
                                util::tuple_all_of(
                                    optional_comp_0_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE(
                                (*std::get<util::OptionalRef<util::meta::const_like_t<
                                     Comps...[0],
                                     std::remove_reference_t<Registry_T>>>>(
                                     optional_comp_0_tuple
                                 )
                                 == comp_0)
                            );

                            decltype(auto
                            ) optional_comp_1_tuple{ static_cast<Registry_T>(registry)
                                                         .template find<Comps...[1]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(optional_comp_1_tuple),
                                          std::tuple<util::OptionalRef<util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>>>>);
                            REQUIRE(
                                util::tuple_all_of(
                                    optional_comp_1_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE(
                                (*std::get<util::OptionalRef<util::meta::const_like_t<
                                     Comps...[1],
                                     std::remove_reference_t<Registry_T>>>>(
                                     optional_comp_1_tuple
                                 )
                                 == comp_1)
                            );

                            decltype(auto) optional_combined_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find<Comps...[0], Comps...[1]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(optional_combined_tuple),
                                          std::tuple<
                                              util::OptionalRef<util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>>,
                                              util::OptionalRef<util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>>>>);
                            REQUIRE(
                                util::tuple_all_of(
                                    optional_combined_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE(
                                util::tuple_all_of(
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
                            static_assert(std::is_same_v<
                                          decltype(optional_shuffled_tuple),
                                          std::tuple<
                                              util::OptionalRef<util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>>,
                                              util::OptionalRef<util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>>>>);
                            REQUIRE(
                                util::tuple_all_of(
                                    optional_shuffled_tuple,
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );
                            REQUIRE(
                                util::tuple_all_of(
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
                                util::tuple_any_of(
                                    static_cast<Registry_T>(registry)
                                        .template find<Comps...[0]>(
                                            core::ecs::Registry::null_id
                                        ),
                                    []<typename Optional_T>(Optional_T optional) static {
                                        return optional.has_value();
                                    }
                                )
                            );

                            REQUIRE_FALSE(
                                util::tuple_any_of(
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
                    util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{ util::meta::name_of<Registry_T>(
                        ) };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto) empty{
                                static_cast<Registry_T>(registry).template find_all<>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(empty),
                                          std::optional<std::tuple<>>>);

                            decltype(auto) optional_comp_0_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(optional_comp_0_tuple),
                                          std::optional<std::tuple<util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>&>>>);
                            REQUIRE(optional_comp_0_tuple.has_value());
                            REQUIRE(
                                (std::get<util::meta::const_like_t<
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
                            static_assert(std::is_same_v<
                                          decltype(optional_comp_1_tuple),
                                          std::optional<std::tuple<util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>&>>>);
                            REQUIRE(optional_comp_1_tuple.has_value());
                            REQUIRE(
                                (std::get<util::meta::const_like_t<
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
                            static_assert(std::is_same_v<
                                          decltype(optional_combined_tuple),
                                          std::optional<std::tuple<
                                              util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>&,
                                              util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>&>>>);
                            REQUIRE(optional_combined_tuple.has_value());
                            REQUIRE(
                                optional_combined_tuple.value()
                                == std::make_tuple(comp_0, comp_1)
                            );

                            decltype(auto) optional_shuffled_tuple{
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[1], Comps...[0]>(id)
                            };
                            static_assert(std::is_same_v<
                                          decltype(optional_shuffled_tuple),
                                          std::optional<std::tuple<
                                              util::meta::const_like_t<
                                                  Comps...[1],
                                                  std::remove_reference_t<Registry_T>>&,
                                              util::meta::const_like_t<
                                                  Comps...[0],
                                                  std::remove_reference_t<Registry_T>>&>>>);
                            REQUIRE(optional_shuffled_tuple.has_value());
                            REQUIRE(
                                optional_shuffled_tuple.value()
                                == std::make_tuple(comp_1, comp_0)
                            );

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_all<Comps...[0]>(
                                        core::ecs::Registry::null_id
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
                    util::meta::for_each<
                        RegistryValueCategories>([&registry]<typename Registry_T> {
                        const std::string section_name{ util::meta::name_of<Registry_T>(
                        ) };
                        SECTION(section_name.c_str())
                        {
                            constexpr static std::type_identity_t<Comps...[0]> comp_0(1);
                            constexpr static std::type_identity_t<Comps...[1]> comp_1(2);

                            const auto id = registry.create(comp_0, comp_1);

                            decltype(auto
                            ) comp_0_result{ static_cast<Registry_T>(registry)
                                                 .template find_single<Comps...[0]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(comp_0_result),
                                          util::OptionalRef<util::meta::const_like_t<
                                              Comps...[0],
                                              std::remove_reference_t<Registry_T>>>>);
                            REQUIRE(comp_0_result.has_value());
                            REQUIRE(comp_0_result.has_value());
                            REQUIRE(*comp_0_result == comp_0);

                            decltype(auto
                            ) comp_1_result{ static_cast<Registry_T>(registry)
                                                 .template find_single<Comps...[1]>(id) };
                            static_assert(std::is_same_v<
                                          decltype(comp_1_result),
                                          util::OptionalRef<util::meta::const_like_t<
                                              Comps...[1],
                                              std::remove_reference_t<Registry_T>>>>);
                            REQUIRE(comp_1_result.has_value());
                            REQUIRE(*comp_1_result == comp_1);

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[0]>(
                                        core::ecs::Registry::null_id
                                    )
                                    .has_value()
                            );

                            REQUIRE_FALSE(
                                static_cast<Registry_T>(registry)
                                    .template find_single<Comps...[2]>(id)
                                    .has_value()
                            );

                            static_assert(!requires {
                                static_cast<Registry_T>(registry).template find_single<>(id
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
                    REQUIRE_FALSE(registry.contains_all<>(core::ecs::Registry::null_id));

                    []<typename... Components_T>() {
                        static_assert(!requires(
                            core::ecs::Registry registry, core::ecs::ID id
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
                        registry.insert(core::ecs::Registry::null_id, Comps...[0](1)),
                        util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.insert(
                            id, Comps...[2](3), Comps...[3](4), Comps...[4](5), Comps...[5](6)
                        ),
                        util::PreconditionViolation
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
                            core::ecs::Registry::null_id, Comps...[0](1)
                        ),
                        util::PreconditionViolation
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

                    decltype(auto) result_1 = registry.remove<Comps...[4], Comps...[5]>(id
                    );
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
                        registry.remove<Comps...[0]>(core::ecs::Registry::null_id),
                        util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.remove<Comps...[4]>(id), util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.remove<Comps...[5]>(id), util::PreconditionViolation
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
                        registry.remove_single<Comps...[0]>(core::ecs::Registry::null_id),
                        util::PreconditionViolation
                    );
                    REQUIRE_THROWS_AS(
                        registry.remove_single<Comps...[5]>(id),
                        util::PreconditionViolation
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
                    static_assert(std::is_same_v<
                                  decltype(result_1),
                                  std::tuple<
                                      std::optional<Comps...[5]>,
                                      std::optional<Comps...[6]>>>);
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
                            registry.erase<Comps...[0]>(core::ecs::Registry::null_id)
                        )
                            .has_value()
                    );
                    REQUIRE_FALSE(
                        std::get<std::optional<Comps...[5]>>(registry.erase<Comps...[5]>(id
                                                             ))
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
                    static_assert(std::is_same_v<
                                  decltype(result_0),
                                  std::optional<std::tuple<>>>);
                    REQUIRE(result_0.has_value());

                    decltype(auto
                    ) result_1 = registry.erase_all<Comps...[4], Comps...[5]>(id);
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
                        registry.erase_all<Comps...[0]>(core::ecs::Registry::null_id)
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
                    static_assert(std::is_same_v<
                                  decltype(result),
                                  std::optional<Comps...[5]>>);
                    REQUIRE(result.has_value());
                    REQUIRE(registry.contains_all<
                            Comps...[0],
                            Comps...[1],
                            Comps...[2],
                            Comps...[3],
                            Comps...[4]>(id));
                    REQUIRE_FALSE(registry.contains_all<Comps...[5]>(id));

                    REQUIRE_FALSE(
                        registry.erase_single<Comps...[0]>(core::ecs::Registry::null_id)
                            .has_value()
                    );
                    REQUIRE_FALSE(registry.erase_single<Comps...[5]>(id).has_value());
                }
            }
        });
    });
}
