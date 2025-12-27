module;

#include <concepts>
#include <format>
#include <utility>

#include "modules/log/log_macros.hpp"

export module ddge.modules.renderer.Plugin;

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.config.engine_name;
import ddge.modules.config.engine_version;
import ddge.modules.log;
import ddge.modules.renderer.Addon;
import ddge.modules.renderer.PluginBuildFailedError;
import ddge.modules.renderer.RenderContextBuilder;
import ddge.modules.resources.Addon;
import ddge.modules.resources.Plugin;
import ddge.modules.vulkan.context;
import ddge.modules.vulkan.InstanceInjection;
import ddge.modules.vulkan.InstanceBuilder;
import ddge.utility.containers.Lazy;
import ddge.utility.Void;

namespace ddge::renderer {

using SupplyVulkanContext = auto (&)() -> const vk::raii::Context&;

template <typename T>
concept render_context_builder_modifier = std::invocable<T&&, RenderContextBuilder&>;

export class Plugin {
public:
    constexpr explicit Plugin(SupplyVulkanContext supply_vulkan_context = vulkan::context);

    template <app::builder_c AppBuilder_T>
    auto setup(AppBuilder_T& app_builder) -> void;

    template <typename Self_T, render_context_builder_modifier Modifier_T>
    auto add_render_context(this Self_T&& self, Modifier_T&& modifier) -> Self_T;

    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) -> app::add_on_t<App_T, Addon>;

private:
    SupplyVulkanContext                 m_supply_vulkan_context;
    std::optional<RenderContextBuilder> m_render_context_builder;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

constexpr Plugin::Plugin(SupplyVulkanContext supply_vulkan_context)
    : m_supply_vulkan_context{ supply_vulkan_context }
{}

auto ensure_instance_builder_precondition(const vk::raii::Context& context)
    -> const vk::raii::Context&
{
    if (!vulkan::InstanceBuilder::check_vulkan_version_support(context)) {
        throw app::PluginSetupFailedError{
            std::format(
                "Required Vulkan version ({}.{}) is not supported. "   //
                "Try upgrading your driver.",
                vk::apiVersionMajor(vulkan::InstanceBuilder::minimum_vulkan_api_version()),
                vk::apiVersionMinor(vulkan::InstanceBuilder::minimum_vulkan_api_version())
            )   //
        };
    }

    return context;
}

[[nodiscard]]
auto try_emplace_instance_injection(
    resources::Plugin&                         resource_plugin,
    const vk::raii::Context&                   context,
    const vulkan::InstanceBuilder::CreateInfo& create_info
) -> vulkan::InstanceInjection&
{
    vulkan::InstanceInjection* instance_injection{};

    resource_plugin.try_inject_resource<vulkan::InstanceInjection>(
        util::Lazy{ [&create_info, &context] -> vulkan::InstanceInjection {
            ensure_instance_builder_precondition(context);
            return vulkan::InstanceInjection{
                vulkan::InstanceBuilder{ create_info, context }
            };
        } },
        &instance_injection
    );

    return *instance_injection;
}

template <app::builder_c AppBuilder_T>
auto Plugin::setup(AppBuilder_T& app_builder) -> void
{
    static_assert(app::has_plugins_c<AppBuilder_T, app::extensions::MetaInfoPlugin>);
    const app::extensions::MetaInfo& meta_info{
        static_cast<const app::extensions::MetaInfoPlugin&>(app_builder).meta_info()
    };
    static_assert(app::has_plugins_c<AppBuilder_T, resources::Plugin>);
    resources::Plugin& resource_plugin{ static_cast<resources::Plugin&>(app_builder) };

    const vulkan::InstanceBuilder::CreateInfo instance_create_info{
        .application_name    = meta_info.application_name(),
        .application_version = meta_info.application_version(),
        .engine_name         = config::engine_name(),
        .engine_version      = vk::makeApiVersion(
            0u,
            config::engine_version().major,
            config::engine_version().minor,
            config::engine_version().patch
        ),
    };

    vulkan::InstanceInjection& instance_injection{ try_emplace_instance_injection(
        resource_plugin, m_supply_vulkan_context(), instance_create_info
    ) };

    m_render_context_builder = RenderContextBuilder{ *instance_injection };
}

template <typename Self_T, render_context_builder_modifier Modifier_T>
auto Plugin::add_render_context(this Self_T&& self, Modifier_T&& modifier) -> Self_T
{
    std::invoke(std::forward<Modifier_T>(modifier), *self.m_render_context_builder);
    return std::forward<Self_T>(self);
}

// ReSharper disable once CppNotAllPathsReturnValue
[[noreturn]]
auto throw_build_failed_error(const RenderContextBuilder::BuildFailure failure)
    -> util::Void
{
    switch (failure) {
        case RenderContextBuilder::BuildFailure::eNoSupportedDeviceFound:
            throw PluginBuildFailedError{
                "No suitable GPU found. Try upgrading your driver."
            };
    }
}

template <ddge::app::decays_to_app_c App_T>
auto Plugin::build(App_T&& app) -> app::add_on_t<App_T, Addon>
{
    static_assert(app::has_addons_c<App_T, app::extensions::MetaInfoAddon>);
    static_assert(app::has_addons_c<App_T, resources::Addon>);
    const resources::Addon& resource_addon{ static_cast<const resources::Addon&>(app) };

    ENGINE_LOG_INFO("Initializing renderer...");

    auto result = std::forward<App_T>(app).add_on(
        Addon{
            .render_context =
                *std::move(m_render_context_builder)
                     ->build(resource_addon.resource_manager.at<vk::raii::Instance>())
                     .transform_error(throw_build_failed_error),
        }
    );

    ENGINE_LOG_INFO(
        std::format(
            "Created renderer for GPU - {}",
            static_cast<const char*>(result.render_context.device.physical_device
                                         .getProperties2()
                                         .properties.deviceName)
        )
    );

    return result;
}

}   // namespace ddge::renderer
