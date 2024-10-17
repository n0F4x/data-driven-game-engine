#include "SurfacePlugin.hpp"

#include "app/App.hpp"
#include "core/renderer/base/instance/Instance.hpp"

namespace plugins::renderer {

auto SurfacePlugin::operator()(App& app, const core::renderer::base::Instance& instance) const
    -> void
{
    const std::optional<VkSurfaceKHR> surface_result{
        std::invoke(m_surface_provider.create_surface, app, instance.get(), nullptr)
    };
    if (!surface_result.has_value()) {
        SPDLOG_ERROR("Vulkan surface creation failed");
        return;
    }

    app.resources.emplace<vk::UniqueSurfaceKHR>(surface_result.value(), instance.get());
}

auto SurfacePlugin::setup(StoreView plugins) const -> void
{
    const std::optional instance_plugin_result{ plugins.find<InstancePlugin>() };
    if (!instance_plugin_result.has_value()) {
        throw std::runtime_error{
            "plugins::renderer::base::InstancePlugin required but not found in 'plugins'"
        };
    }
    InstancePlugin& instance_plugin{ instance_plugin_result.value().get() };

    instance_plugin.emplace_dependency(m_surface_provider.instance_dependencies);
}

}   // namespace plugins::renderer
