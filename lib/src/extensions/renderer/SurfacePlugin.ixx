module;

export module extensions.renderer.SurfacePlugin;

import vulkan_hpp;

import core.renderer.base.instance.Instance;

import core.window.Window;

import extensions.renderer.InstancePlugin;

namespace extensions::renderer {

export class SurfacePlugin {
public:
    [[nodiscard]]
    auto operator()(
        const core::window::Window&           window,
        const core::renderer::base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(InstancePlugin& instance_plugin) -> void;
};

}   // namespace extensions::renderer
