module;

#include <functional>

export module demos.virtual_texture;

import vulkan_hpp;

import ddge.modules.app.has_addons_c;
import ddge.modules.gfx.Camera;
import ddge.modules.resources.Addon;

import examples.base.DemoBase;
import examples.base.Renderer;

import demos.virtual_texture.DemoApp;

namespace demo {

export constexpr inline auto run =
    []<ddge::app::has_addons_c<ddge::resources::Addon> App_T>(App_T&& app) -> void {
    app.resource_manager.template at<examples::base::DemoBase>().run(
        [demo_app = std::ref(app.resource_manager.template at<DemoApp>())](
            examples::base::Renderer& renderer,
            const vk::Extent2D        framebuffer_size,
            ddge::gfx::Camera         camera
        ) {
            renderer.render(
                framebuffer_size,
                camera,
                std::bind_front(&DemoApp::render, std::ref(demo_app))
            );
        }
    );
};

}   // namespace demo
