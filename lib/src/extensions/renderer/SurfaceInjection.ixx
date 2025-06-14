module;

export module extensions.renderer.SurfaceInjection;

import vulkan_hpp;

import core.renderer.base.instance.Instance;

import core.window.Window;

import extensions.renderer.InstanceInjection;

namespace extensions::renderer {

export class SurfaceInjection {
public:
    [[nodiscard]]
    auto operator()(
        const core::window::Window&           window,
        const core::renderer::base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(InstanceInjection& instance_injection) -> void;
};

}   // namespace extensions::renderer
