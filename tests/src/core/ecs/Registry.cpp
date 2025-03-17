#include <entt/core/type_info.hpp>

#include <catch2/catch_test_macros.hpp>

#include "utility/contracts.hpp"

import core.ecs;
import utility.meta.type_traits.forward_like;
import utility.TypeList;

using namespace std::literals;

TEST_CASE("core::ecs::Registry")
{
    using Registry = core::ecs::Registry<>;
    Registry registry;

    const util::TypeList<Registry&, const Registry&, Registry&&, const Registry&&>
        value_categorized_registries{};

    SECTION("create")
    {
        decltype(auto) id = registry.create(int{}, float{});

        static_assert(std::is_same_v<decltype(id), core::ecs::ID<Registry>>);
    }

    value_categorized_registries.for_each([&registry]<typename Registry_T> {
        const std::string section_name{ "get - "s + entt::type_name<Registry_T>::value() };

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
                std::get<util::meta::forward_like_t<int, Registry_T>>(integer_tuple)
                == integer
            );

            decltype(auto
            ) floating_tuple{ static_cast<Registry_T>(registry).template get<float>(id) };
            static_assert(std::is_same_v<
                          decltype(floating_tuple),
                          std::tuple<util::meta::forward_like_t<float, Registry_T>>>);
            REQUIRE(
                std::get<util::meta::forward_like_t<float, Registry_T>>(floating_tuple)
                == floating
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
                static_cast<Registry_T>(registry).template get<int>(Registry::null_id),
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

    value_categorized_registries.for_each([&registry]<typename Registry_T> {
        const std::string section_name{ "get_single - "s
                                        + entt::type_name<Registry_T>::value() };
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
                    Registry::null_id
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
