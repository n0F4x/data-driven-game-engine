module;

export module ddge.modules.renderer.SurfaceInjection;

import vulkan_hpp;

import ddge.modules.renderer.base.instance.Instance;

import ddge.modules.window.Window;

import ddge.modules.renderer.InstanceInjection;

namespace ddge::renderer {

export class SurfaceInjection {
public:
    [[nodiscard]]
    auto operator()(const window::Window& window, const base::Instance& instance) const
        -> vk::UniqueSurfaceKHR;

    static auto setup(InstanceInjection& instance_injection) -> void;
};

}   // namespace ddge::renderer
