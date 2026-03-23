module;

#include <cassert>
#include <concepts>
#include <expected>
#include <format>
#include <functional>
#include <utility>

export module ddge.modules.renderer.Plugin;

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.config.engine_name;
import ddge.modules.config.engine_version;
import ddge.modules.renderer.ContextInjection;
import ddge.modules.renderer.vulkan_profile;
import ddge.modules.resources.Plugin;
import ddge.modules.vulkan.context;
import ddge.modules.vulkan.DeviceBuilder;
import ddge.modules.vulkan.InstanceInjection;
import ddge.modules.vulkan.InstanceBuilder;
import ddge.modules.wsi.Context;
import ddge.modules.wsi.vulkan_instance_extensions;
import ddge.util.containers.Lazy;
import ddge.util.containers.OptionalRef;
import ddge.util.containers.StringLiteral;

namespace ddge::renderer {

using SupplyVulkanContext = auto (&)() -> const vk::raii::Context&;

template <typename T>
concept render_context_builder_modifier = std::invocable<T&&, ContextInjection&>;

export class Plugin {
public:
    struct CreateInfo {
        SupplyVulkanContext supply_vulkan_context{ vulkan::context };
        bool                headless{};
    };

    constexpr Plugin();
    constexpr explicit Plugin(const CreateInfo& create_info);

    template <app::builder_c AppBuilder_T>
    auto setup(AppBuilder_T& app_builder) -> void;

    template <typename Self_T, render_context_builder_modifier Modifier_T>
    auto add_render_context(this Self_T&& self, Modifier_T&& modifier) -> Self_T;

private:
    SupplyVulkanContext                 m_supply_vulkan_context;
    bool                                m_headless;
    util::OptionalRef<ContextInjection> m_render_context_injection_ref;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

constexpr Plugin::Plugin() : Plugin{ CreateInfo{} } {}

constexpr Plugin::Plugin(const CreateInfo& create_info)
    : m_supply_vulkan_context{ create_info.supply_vulkan_context },
      m_headless{ create_info.headless }
{}

auto ensure_instance_builder_precondition(const vk::raii::Context& context)
    -> const vk::raii::Context&
{
    if (!vulkan::InstanceBuilder::check_version_support(context)) {
        throw app::PluginSetupFailedError{
            std::format(
                "Required Vulkan version ({}.{}) is not supported. "   //
                "Try upgrading your driver.",
                vk::apiVersionMajor(vulkan::InstanceBuilder::minimum_version()),
                vk::apiVersionMinor(vulkan::InstanceBuilder::minimum_version())
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
    instance_injection->request_api_version(vulkan_profile().api_version);

    if (!m_headless) {
        wsi::Context wsi_context;

        if (const std::expected<
                std::span<const util::StringLiteral>,
                wsi::VulkanSurfaceCreationNotSupportedError> expected_surface_extensions{
                wsi::vulkan_instance_extensions(wsi_context) };
            expected_surface_extensions.has_value())
        {
            resource_plugin.try_emplace_resource<wsi::Context>(wsi_context);

            for (const util::StringLiteral extension_name : *expected_surface_extensions)
            {
                {
                    [[maybe_unused]]
                    const bool success =
                        instance_injection->enable_extension(extension_name);
                    assert(success);
                }
            }
        }
        else {
            m_headless = true;
        }
    }

    ContextInjection* context_injection{};
    resource_plugin.inject_resource(
        ContextInjection{ *instance_injection }, &context_injection
    );
    m_render_context_injection_ref = *context_injection;

    m_render_context_injection_ref->device_builder().require_and_enable_capabilities(
        vulkan_profile().physical_device_capabilities
    );

    if (!m_headless) {
        m_render_context_injection_ref->request_wsi_support();
    }
}

template <typename Self_T, render_context_builder_modifier Modifier_T>
auto Plugin::add_render_context(this Self_T&& self, Modifier_T&& modifier) -> Self_T
{
    std::invoke(std::forward<Modifier_T>(modifier), *self.m_render_context_injection_ref);
    return std::forward<Self_T>(self);
}

}   // namespace ddge::renderer
