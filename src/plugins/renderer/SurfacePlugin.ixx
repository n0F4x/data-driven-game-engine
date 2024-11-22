module;

export module plugins.renderer.SurfacePlugin;

import vulkan_hpp;

import store.StoreView;

import core.renderer.base.instance.Instance;

import core.window.Window;

namespace plugins::renderer {

export class SurfacePlugin {
public:
    [[nodiscard]]
    auto operator()(
        const core::window::Window&           window,
        const core::renderer::base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(StoreView plugins) -> void;
};

}   // namespace plugins::renderer
