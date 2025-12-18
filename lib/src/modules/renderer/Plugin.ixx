module;

#include <concepts>
#include <format>
#include <type_traits>
#include <utility>
#include <vector>

#include "modules/log/log_macros.hpp"

export module ddge.modules.renderer.Plugin;

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.log;
import ddge.modules.renderer.Addon;
import ddge.modules.renderer.PluginBuildFailedError;
import ddge.modules.renderer.RenderContextBuilder;
import ddge.modules.vulkan.context;
import ddge.utility.containers.AnyCopyableFunction;
import ddge.utility.meta.concepts.naked;
import ddge.utility.Void;

namespace ddge::renderer {

using SupplyVulkanContext = auto (&)() -> const vk::raii::Context&;

template <typename T>
concept render_context_builder_modifier = util::meta::naked_c<T> && std::copyable<T>
                                       && std::invocable<const T, RenderContextBuilder&>;

template <typename T>
concept decays_to_render_context_builder_modifier =
    render_context_builder_modifier<std::decay_t<T>>;

export class Plugin {
public:
    constexpr explicit Plugin(SupplyVulkanContext supply_vulkan_context = vulkan::context);

    template <typename Self_T, decays_to_render_context_builder_modifier Modifier_T>
    constexpr auto add_render_context(this Self_T&& self, Modifier_T&& modifier)
        -> Self_T;

    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) -> app::add_on_t<App_T, Addon>;

private:
    using Modifier = util::AnyCopyableFunction<auto(RenderContextBuilder&) const->void>;

    SupplyVulkanContext   m_supply_vulkan_context;
    std::vector<Modifier> m_modifiers;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

constexpr Plugin::Plugin(SupplyVulkanContext supply_vulkan_context)
    : m_supply_vulkan_context{ supply_vulkan_context }
{}

template <typename Self_T, decays_to_render_context_builder_modifier Modifier_T>
constexpr auto Plugin::add_render_context(this Self_T&& self, Modifier_T&& modifier)
    -> Self_T
{
    self.Plugin::m_modifiers.emplace_back(std::forward<Modifier_T>(modifier));
    return std::forward<Self_T>(self);
}

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

    const RenderContextBuilder::CreateInfo render_context_builder_create_info{
        .application_name    = app.meta_info.application_name(),
        .application_version = app.meta_info.application_version(),
    };

    RenderContextBuilder render_context_builder{
        render_context_builder_create_info,
        m_supply_vulkan_context(),
    };

    for (const Modifier& modifier : m_modifiers) {
        modifier(render_context_builder);
    }

    ENGINE_LOG_INFO("Initializing renderer...");

    auto result = std::forward<App_T>(app).add_on(
        Addon{
            .render_context = *std::move(render_context_builder)
                                   .build()
                                   .transform_error(throw_build_failed_error),
        }
    );

    ENGINE_LOG_INFO(
        std::format(
            "Created renderer for GPU - {}",
            static_cast<const char*>(
                result.render_context.physical_device.getProperties2().properties.deviceName
            )
        )
    );

    return result;
}

}   // namespace ddge::renderer
