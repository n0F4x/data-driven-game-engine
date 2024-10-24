#pragma once

#include <optional>

#include "InstancePlugin.hpp"

class App;
class StoreView;

namespace core::renderer::base {

class Instance;

}   // namespace core::renderer::base

namespace plugins::renderer {

class SurfacePlugin {
public:
    struct SurfaceProvider {
        std::function<std::optional<
            VkSurfaceKHR>(const App&, VkInstance, const VkAllocationCallbacks*)>
                                   create_surface;
        InstancePlugin::Dependency instance_dependencies;
    };

    SurfacePlugin() = default;
    template <typename... Args>
    explicit SurfacePlugin(std::in_place_t, Args&&... args);

    auto operator()(App& app, const core::renderer::base::Instance&) const -> void;

    auto setup(StoreView plugins) const -> void;

private:
    SurfaceProvider m_surface_provider;
};

}   // namespace plugins::renderer

#include "SurfacePlugin.inl"
