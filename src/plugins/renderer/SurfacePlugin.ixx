module;

export module plugins.renderer.SurfacePlugin;

import vulkan_hpp;

import core.renderer.base.instance.Instance;

import core.window.Window;

import plugins.renderer.InstancePlugin;

namespace plugins::renderer {

export class SurfacePlugin {
public:
    [[nodiscard]]
    auto operator()(
        const core::window::Window&           window,
        const core::renderer::base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(InstancePlugin& instance_plugin) -> void;
};

}   // namespace plugins::renderer
