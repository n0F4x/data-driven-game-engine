#include <catch2/catch_test_macros.hpp>

#include "utility/contracts.hpp"

import core.ecs;
import utility.containers.OptionalRef;
import utility.meta.algorithms.for_each;
import utility.meta.reflection.name_of;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.underlying;
import utility.tuple.tuple_all_of;
import utility.tuple.tuple_any_of;
import utility.TypeList;

using namespace std::literals;

TEST_CASE("core::ecs::Registry")
{
    core::ecs::Registry registry;

    using RegistryValueCategories = util::TypeList<
        core::ecs::Registry&,
        const core::ecs::Registry&,
        core::ecs::Registry&&,
        const core::ecs::Registry&&>;

    SECTION("create")
    {
        decltype(auto) id = registry.create(int{}, float{});

        static_assert(std::is_same_v<decltype(id), core::ecs::ID>);
    }

    SECTION("destroy")
    {
        const auto id = registry.create(int{}, float{});

        const bool successfully_destroyed_contained = registry.destroy(id);
        REQUIRE(successfully_destroyed_contained);

        const bool successfully_destroyed_non_contained = registry.destroy(id);
        REQUIRE_FALSE(successfully_destroyed_non_contained);
    }

    util::meta::for_each<RegistryValueCategories>([&registry]<typename Registry_T> {
        const std::string section_name{ "get - "s + util::meta::name_of<Registry_T>() };

        SECTION(section_name.c_str())
        {
            constexpr static int   integer{ 1 };
            constexpr static float floating{ 2 };

            const auto id = registry.create(integer, floating);

            decltype(auto) empty{ static_cast<Registry_T>(registry).template get<>(id) };
            static_assert(std::is_same_v<decltype(empty), std::tuple<>>);

            decltype(auto
            ) integer_tuple{ static_cast<Registry_T>(registry).template get<int>(id) };
            static_assert(std::is_same_v<
                          decltype(integer_tuple),
                          std::tuple<util::meta::forward_like_t<int, Registry_T>>>);
            REQUIRE(
                (std::get<util::meta::forward_like_t<int, Registry_T>>(integer_tuple)
                 == integer)
            );

            decltype(auto
            ) floating_tuple{ static_cast<Registry_T>(registry).template get<float>(id) };
            static_assert(std::is_same_v<
                          decltype(floating_tuple),
                          std::tuple<util::meta::forward_like_t<float, Registry_T>>>);
            REQUIRE(
                (std::get<util::meta::forward_like_t<float, Registry_T>>(floating_tuple)
                 == floating)
            );

            decltype(auto) combined_tuple{
                static_cast<Registry_T>(registry).template get<int, float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(combined_tuple),
                          std::tuple<
                              util::meta::forward_like_t<int, Registry_T>,
                              util::meta::forward_like_t<float, Registry_T>>>);
            REQUIRE(combined_tuple == std::make_tuple(integer, floating));

            decltype(auto) shuffled_tuple{
                static_cast<Registry_T>(registry).template get<float, int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(shuffled_tuple),
                          std::tuple<
                              util::meta::forward_like_t<float, Registry_T>,
                              util::meta::forward_like_t<int, Registry_T>>>);
            REQUIRE(shuffled_tuple == std::make_tuple(floating, integer));

            REQUIRE_THROWS_AS(
                static_cast<Registry_T>(registry).template get<int>(
                    core::ecs::Registry::null_id
                ),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                static_cast<Registry_T>(registry).template get<double>(id),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                (static_cast<Registry_T>(registry).template get<int, float, double>(id)),
                util::PreconditionViolation
            );

            static_assert(!requires {
                static_cast<Registry_T>(registry).template get<int, float, int>(id);
            });
        }
    });

    util::meta::for_each<RegistryValueCategories>([&registry]<typename Registry_T> {
        const std::string section_name{ "get_single - "s
                                        + util::meta::name_of<Registry_T>() };
        SECTION(section_name.c_str())
        {
            constexpr static int   integer{ 1 };
            constexpr static float floating{ 2 };

            const auto id = registry.create(integer, floating);

            decltype(auto) integer_result{
                static_cast<Registry_T>(registry).template get_single<int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(integer_result),
                          util::meta::forward_like_t<int, Registry_T>>);
            REQUIRE(integer_result == integer);

            decltype(auto) floating_result{
                static_cast<Registry_T>(registry).template get_single<float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(floating_result),
                          util::meta::forward_like_t<float, Registry_T>>);
            REQUIRE(floating_result == floating);

            REQUIRE_THROWS_AS(
                static_cast<Registry_T>(registry).template get_single<int>(
                    core::ecs::Registry::null_id
                ),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                static_cast<Registry_T>(registry).template get_single<double>(id),
                util::PreconditionViolation
            );

            static_assert(!requires {
                static_cast<Registry_T>(registry).template get_single<>(id);
            });

            static_assert(!requires {
                static_cast<Registry_T>(registry).template get_single<int, float>(id);
            });
        }
    });

    util::meta::for_each<RegistryValueCategories>([&registry]<typename Registry_T> {
        const std::string section_name{ "find - "s + util::meta::name_of<Registry_T>() };

        SECTION(section_name.c_str())
        {
            constexpr static int   integer{ 1 };
            constexpr static float floating{ 2 };

            const auto id = registry.create(integer, floating);

            decltype(auto) empty{ static_cast<Registry_T>(registry).template find<>(id) };
            static_assert(std::is_same_v<decltype(empty), std::tuple<>>);

            decltype(auto) optional_integer_tuple{
                static_cast<Registry_T>(registry).template find<int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_integer_tuple),
                          std::tuple<util::OptionalRef<std::remove_reference_t<
                              util::meta::forward_like_t<int, Registry_T>>>>>);
            REQUIRE(
                util::tuple_all_of(
                    optional_integer_tuple,
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return optional.has_value();
                    }
                )
            );
            REQUIRE((
                *std::get<util::OptionalRef<std::remove_reference_t<
                    util::meta::forward_like_t<int, Registry_T>>>>(optional_integer_tuple)
                == integer
            ));

            decltype(auto) optional_floating_tuple{
                static_cast<Registry_T>(registry).template find<float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_floating_tuple),
                          std::tuple<util::OptionalRef<std::remove_reference_t<
                              util::meta::forward_like_t<float, Registry_T>>>>>);
            REQUIRE(
                util::tuple_all_of(
                    optional_floating_tuple,
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return optional.has_value();
                    }
                )
            );
            REQUIRE(
                (*std::get<util::OptionalRef<std::remove_reference_t<
                     util::meta::forward_like_t<float, Registry_T>>>>(
                     optional_floating_tuple
                 )
                 == floating)
            );

            decltype(auto) optional_combined_tuple{
                static_cast<Registry_T>(registry).template find<int, float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_combined_tuple),
                          std::tuple<
                              util::OptionalRef<std::remove_reference_t<
                                  util::meta::forward_like_t<int, Registry_T>>>,
                              util::OptionalRef<std::remove_reference_t<
                                  util::meta::forward_like_t<float, Registry_T>>>>>);
            REQUIRE(
                util::tuple_all_of(
                    optional_combined_tuple,
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return optional.has_value();
                    }
                )
            );
            REQUIRE(
                util::tuple_all_of(
                    optional_combined_tuple,
                    []<typename T>(T&& value) static {
                        if constexpr (std::is_same_v<std::remove_cvref_t<T>, int>) {
                            return value == integer;
                        }
                        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>)
                        {
                            return value == floating;
                        }
                    },
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return *optional;
                    }
                )
            );

            decltype(auto) optional_shuffled_tuple{
                static_cast<Registry_T>(registry).template find<float, int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_shuffled_tuple),
                          std::tuple<
                              util::OptionalRef<std::remove_reference_t<
                                  util::meta::forward_like_t<float, Registry_T>>>,
                              util::OptionalRef<std::remove_reference_t<
                                  util::meta::forward_like_t<int, Registry_T>>>>>);
            REQUIRE(
                util::tuple_all_of(
                    optional_shuffled_tuple,
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return optional.has_value();
                    }
                )
            );
            REQUIRE(
                util::tuple_all_of(
                    optional_shuffled_tuple,
                    []<typename T>(T&& value) static {
                        if constexpr (std::is_same_v<std::remove_cvref_t<T>, int>) {
                            return value == integer;
                        }
                        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, float>)
                        {
                            return value == floating;
                        }
                    },
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return *optional;
                    }
                )
            );

            REQUIRE_FALSE(
                util::tuple_any_of(
                    static_cast<Registry_T>(registry).template find<int>(
                        core::ecs::Registry::null_id
                    ),
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return optional.has_value();
                    }
                )
            );

            REQUIRE_FALSE(
                util::tuple_any_of(
                    static_cast<Registry_T>(registry).template find<double>(id),
                    []<typename Optional_T>(Optional_T&& optional) static {
                        return optional.has_value();
                    }
                )
            );

            static_assert(!requires {
                static_cast<Registry_T>(registry).template find<int, float, int>(id);
            });
        }
    });

    util::meta::for_each<RegistryValueCategories>([&registry]<typename Registry_T> {
        const std::string section_name{ "find_all - "s
                                        + util::meta::name_of<Registry_T>() };

        SECTION(section_name.c_str())
        {
            constexpr static int   integer{ 1 };
            constexpr static float floating{ 2 };

            const auto id = registry.create(integer, floating);

            decltype(auto
            ) empty{ static_cast<Registry_T>(registry).template find_all<>(id) };
            static_assert(std::is_same_v<decltype(empty), std::optional<std::tuple<>>>);

            decltype(auto) optional_integer_tuple{
                static_cast<Registry_T>(registry).template find_all<int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_integer_tuple),
                          std::optional<
                              std::tuple<util::meta::forward_like_t<int, Registry_T>>>>);
            REQUIRE(optional_integer_tuple.has_value());
            REQUIRE(
                (std::get<util::meta::forward_like_t<int, Registry_T>>(
                     optional_integer_tuple.value()
                 )
                 == integer)
            );

            decltype(auto) optional_floating_tuple{
                static_cast<Registry_T>(registry).template find_all<float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_floating_tuple),
                          std::optional<
                              std::tuple<util::meta::forward_like_t<float, Registry_T>>>>);
            REQUIRE(optional_floating_tuple.has_value());
            REQUIRE(
                (std::get<util::meta::forward_like_t<float, Registry_T>>(
                     optional_floating_tuple.value()
                 )
                 == floating)
            );

            decltype(auto) optional_combined_tuple{
                static_cast<Registry_T>(registry).template find_all<int, float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_combined_tuple),
                          std::optional<std::tuple<
                              util::meta::forward_like_t<int, Registry_T>,
                              util::meta::forward_like_t<float, Registry_T>>>>);
            REQUIRE(optional_combined_tuple.has_value());
            REQUIRE(optional_combined_tuple.value() == std::make_tuple(integer, floating));

            decltype(auto) optional_shuffled_tuple{
                static_cast<Registry_T>(registry).template find_all<float, int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(optional_shuffled_tuple),
                          std::optional<std::tuple<
                              util::meta::forward_like_t<float, Registry_T>,
                              util::meta::forward_like_t<int, Registry_T>>>>);
            REQUIRE(optional_shuffled_tuple.has_value());
            REQUIRE(optional_shuffled_tuple.value() == std::make_tuple(floating, integer));

            REQUIRE_FALSE(
                static_cast<Registry_T>(registry)
                    .template find_all<int>(core::ecs::Registry::null_id)
                    .has_value()
            );

            REQUIRE_FALSE(
                static_cast<Registry_T>(registry).template find_all<double>(id).has_value()
            );

            REQUIRE_FALSE((static_cast<Registry_T>(registry)
                               .template find_all<int, float, double>(id)
                               .has_value()));

            static_assert(!requires {
                static_cast<Registry_T>(registry).template find_all<int, float, int>(id);
            });
        }
    });

    util::meta::for_each<RegistryValueCategories>([&registry]<typename Registry_T> {
        const std::string section_name{ "find_single - "s
                                        + util::meta::name_of<Registry_T>() };
        SECTION(section_name.c_str())
        {
            constexpr static int   integer{ 1 };
            constexpr static float floating{ 2 };

            const auto id = registry.create(integer, floating);

            decltype(auto) integer_result{
                static_cast<Registry_T>(registry).template find_single<int>(id)
            };
            static_assert(std::is_same_v<
                          decltype(integer_result),
                          util::OptionalRef<std::remove_reference_t<
                              util::meta::forward_like_t<int, Registry_T>>>>);
            REQUIRE(integer_result.has_value());
            REQUIRE(integer_result.has_value());
            REQUIRE(*integer_result == integer);

            decltype(auto) floating_result{
                static_cast<Registry_T>(registry).template find_single<float>(id)
            };
            static_assert(std::is_same_v<
                          decltype(floating_result),
                          util::OptionalRef<std::remove_reference_t<
                              util::meta::forward_like_t<float, Registry_T>>>>);
            REQUIRE(floating_result.has_value());
            REQUIRE(*floating_result == floating);

            REQUIRE_FALSE(
                static_cast<Registry_T>(registry)
                    .template find_single<int>(core::ecs::Registry::null_id)
                    .has_value()
            );

            REQUIRE_FALSE(
                static_cast<Registry_T>(registry)
                    .template find_single<double>(id)
                    .has_value()
            );

            static_assert(!requires {
                static_cast<Registry_T>(registry).template find_single<>(id);
            });

            static_assert(!requires {
                static_cast<Registry_T>(registry).template find_single<int, float>(id);
            });
        }
    });

    SECTION("contains_all")
    {
        const auto id = registry.create(int{}, float{});

        REQUIRE(registry.contains_all<>(id));
        REQUIRE(registry.contains_all<int>(id));
        REQUIRE(registry.contains_all<float>(id));
        REQUIRE(registry.contains_all<int, float>(id));
        REQUIRE(registry.contains_all<float, int>(id));

        REQUIRE_FALSE(registry.contains_all<float, int, double>(id));
        REQUIRE_FALSE(registry.contains_all<double>(id));
        REQUIRE_FALSE(registry.contains_all<>(core::ecs::Registry::null_id));

        static_assert([]<typename... Components_T>() static {
            return !requires { registry.contains_all<Components_T...>(id); };
        }.operator()<int, float, int>());
    }

    SECTION("insert")
    {
        const auto id = registry.create(float{}, double{});

        registry.insert(id, int8_t{}, int16_t{});
        REQUIRE(registry.contains_all<float, double, int8_t, int16_t>(id));

        REQUIRE_THROWS_AS(
            registry.insert(id, int8_t{}, int16_t{}, int32_t{}, int64_t{}),
            util::PreconditionViolation
        );
    }

    SECTION("insert_or_replace")
    {
        const auto id = registry.create(float{}, double{});

        registry.insert_or_replace(id, int8_t{}, int16_t{});
        REQUIRE(registry.contains_all<float, double, int8_t, int16_t>(id));

        registry.insert_or_replace(id, int8_t{}, int16_t{}, int32_t{}, int64_t{});
        REQUIRE(   //
            registry.contains_all<float, double, int8_t, int16_t, int32_t, int64_t>(id)
        );
    }
}
