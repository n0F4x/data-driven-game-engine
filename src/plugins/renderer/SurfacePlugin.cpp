#include "SurfacePlugin.hpp"

#include "app/App.hpp"
#include "core/renderer/base/instance/Instance.hpp"

namespace plugins::renderer {

auto SurfacePlugin::operator()(App& app) const -> void
{
    const std::optional instance_result{ app.resources.find<core::renderer::Instance>() };
    if (!instance_result.has_value()) {
        throw std::runtime_error{ "Instance required but not found in 'app.resources'" };
    }
    const core::renderer::Instance& instance{ instance_result.value().get() };

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
            "plugins::renderer::InstancePlugin required but not found in 'plugins'"
        };
    }
    InstancePlugin& instance_plugin{ instance_plugin_result.value().get() };

    instance_plugin.emplace_dependency(std::move(m_surface_provider).instance_dependencies
    );
}

}   // namespace plugins::renderer
