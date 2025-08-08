module;

export module core.renderer.SurfaceInjection;

import vulkan_hpp;

import core.renderer.base.instance.Instance;

import core.window.Window;

import core.renderer.InstanceInjection;

namespace core::renderer {

export class SurfaceInjection {
public:
    [[nodiscard]]
    auto operator()(
        const core::window::Window&           window,
        const core::renderer::base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(InstanceInjection& instance_injection) -> void;
};

}   // namespace core::renderer
