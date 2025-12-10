#include <expected>
#include <type_traits>
#include <variant>

#include <catch2/catch_test_macros.hpp>

import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.result.Result;
import ddge.modules.vulkan.result.TypedResultCode;
import ddge.modules.vulkan.result.VulkanError;
import ddge.utility.contracts;

import vulkan_hpp;

using namespace ddge;

TEST_CASE("ddge::vulkan::check_result")
{
    SECTION("vk::Result")
    {
        SECTION("no expected result code")
        {
            static_assert(
                std::is_same_v<decltype(vulkan::check_result(vk::Result::eSuccess)), void>
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result(vk::Result::eErrorExtensionNotPresent),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result(vk::Result::eErrorDeviceLost),
                vulkan::VulkanError
            );

            vulkan::check_result(vk::Result::eSuccess);
        }

        SECTION("expected result code")
        {
            static_assert(std::is_same_v<
                          decltype(vulkan::check_result<
                                   vk::Result::eIncomplete>(vk::Result::eSuccess)),
                          std::variant<
                              vulkan::TypedResultCode<vk::Result::eSuccess>,
                              vulkan::TypedResultCode<vk::Result::eIncomplete>>>);

            REQUIRE_THROWS_AS(
                vulkan::check_result<vk::Result::eIncomplete>(
                    vk::Result::eErrorExtensionNotPresent
                ),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result<vk::Result::eIncomplete>(
                    vk::Result::eErrorDeviceLost
                ),
                vulkan::VulkanError
            );

            REQUIRE(
                std::holds_alternative<vulkan::TypedResultCode<vk::Result::eSuccess>>(
                    vulkan::check_result<vk::Result::eErrorDeviceLost>(
                        vk::Result::eSuccess
                    )
                )
            );

            REQUIRE(
                std::holds_alternative<
                    vulkan::TypedResultCode<vk::Result::eErrorDeviceLost>>(
                    vulkan::check_result<vk::Result::eErrorDeviceLost>(
                        vk::Result::eErrorDeviceLost
                    )
                )
            );
        }
    }

    SECTION("vk::ResultValue<T>")
    {
        SECTION("no expected result code")
        {
            static_assert(std::is_same_v<
                          decltype(vulkan::check_result(
                              std::declval<vk::ResultValue<vk::Instance>>()
                          )),
                          vk::Instance>);

            REQUIRE_THROWS_AS(
                vulkan::check_result(
                    vk::ResultValue<vk::Instance>{
                        vk::Result::eErrorExtensionNotPresent,
                        nullptr,
                    }
                ),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result(
                    vk::ResultValue<vk::Instance>{
                        vk::Result::eErrorDeviceLost,
                        nullptr,
                    }
                ),
                vulkan::VulkanError
            );

            std::ignore = vulkan::check_result(
                vk::ResultValue<vk::Instance>{
                    vk::Result::eSuccess,
                    nullptr,
                }
            );
        }

        SECTION("expected result code")
        {
            static_assert(
                std::is_same_v<
                    decltype(vulkan::check_result<vk::Result::eIncomplete>(
                        std::declval<vk::ResultValue<vk::Instance>>()
                    )),
                    vulkan::
                        Result<vk::Instance, vk::Result::eSuccess, vk::Result::eIncomplete>>
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result<vk::Result::eIncomplete>(vk::ResultValue<vk::Instance>{
                    vk::Result::eErrorExtensionNotPresent,
                    nullptr,
                }),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result<vk::Result::eIncomplete>(vk::ResultValue<vk::Instance>{
                    vk::Result::eErrorDeviceLost,
                    nullptr,
                }),
                vulkan::VulkanError
            );

            REQUIRE(
                std::holds_alternative<vulkan::TypedResultCode<vk::Result::eSuccess>>(
                    vulkan::check_result<vk::Result::eErrorDeviceLost>(
                        vk::ResultValue<vk::Instance>{
                            vk::Result::eSuccess,
                            nullptr,
                        }
                    )
                        .result_code()
                )
            );

            REQUIRE(
                std::holds_alternative<
                    vulkan::TypedResultCode<vk::Result::eErrorDeviceLost>>(
                    vulkan::check_result<vk::Result::eErrorDeviceLost>(
                        vk::ResultValue<vk::Instance>{
                            vk::Result::eErrorDeviceLost,
                            nullptr,
                        }
                    )
                        .result_code()
                )
            );
        }
    }

    SECTION("std::expected<T, vk::Result>")
    {
        SECTION("no expected result code")
        {
            static_assert(std::is_same_v<
                          decltype(vulkan::check_result(
                              std::declval<std::expected<vk::Instance, vk::Result>>()
                          )),
                          vk::Instance>);

            REQUIRE_THROWS_AS(
                vulkan::check_result(
                    std::expected<vk::Instance, vk::Result>{
                        std::unexpect,
                        vk::Result::eErrorExtensionNotPresent,
                    }
                ),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result(
                    std::expected<vk::Instance, vk::Result>{
                        std::unexpect,
                        vk::Result::eErrorDeviceLost,
                    }
                ),
                vulkan::VulkanError
            );

            std::ignore =
                vulkan::check_result(std::expected<vk::Instance, vk::Result>{ nullptr });
        }

        SECTION("expected result code")
        {
            static_assert(
                std::is_same_v<
                    decltype(vulkan::check_result<vk::Result::eErrorDeviceLost>(
                        std::declval<std::expected<vk::Instance, vk::Result>>()
                    )),
                    std::expected<
                        vk::Instance,
                        std::variant<
                            vulkan::TypedResultCode<vk::Result::eErrorDeviceLost>>>>
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result<vk::Result::eIncomplete>(
                    std::expected<vk::Instance, vk::Result>{
                        std::unexpect,
                        vk::Result::eErrorExtensionNotPresent,
                    }
                ),
                util::PreconditionViolation
            );

            REQUIRE_THROWS_AS(
                vulkan::check_result<vk::Result::eIncomplete>(
                    std::expected<vk::Instance, vk::Result>{
                        std::unexpect,
                        vk::Result::eErrorDeviceLost,
                    }
                ),
                vulkan::VulkanError
            );

            REQUIRE(
                vulkan::check_result<vk::Result::eErrorDeviceLost>(
                    std::expected<vk::Instance, vk::Result>{ nullptr }
                )
                    .has_value()
            );

            REQUIRE(   //
                !vulkan::check_result<vk::Result::eErrorDeviceLost>(
                     std::expected<vk::Instance, vk::Result>{
                         std::unexpect,
                         vk::Result::eErrorDeviceLost,
                     }
                )
                     .has_value()
            );
        }
    }
}
