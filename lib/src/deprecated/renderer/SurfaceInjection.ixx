module;

export module ddge.deprecated.renderer.SurfaceInjection;

import vulkan_hpp;

import ddge.deprecated.renderer.base.instance.Instance;

import ddge.deprecated.window.Window;

import ddge.deprecated.renderer.InstanceInjection;

namespace ddge::renderer {

export class SurfaceInjection {
public:
    [[nodiscard]]
    auto operator()(const window::Window& window, const base::Instance& instance) const
        -> vk::UniqueSurfaceKHR;

    static auto setup(InstanceInjection& instance_injection) -> void;
};

}   // namespace ddge::renderer
