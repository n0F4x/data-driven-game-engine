module;

export module modules.renderer.SurfaceInjection;

import vulkan_hpp;

import modules.renderer.base.instance.Instance;

import modules.window.Window;

import modules.renderer.InstanceInjection;

namespace modules::renderer {

export class SurfaceInjection {
public:
    [[nodiscard]]
    auto operator()(
        const window::Window&           window,
        const base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(InstanceInjection& instance_injection) -> void;
};

}   // namespace modules::renderer
