module;

#include <functional>

export module demos.virtual_texture;

import vulkan_hpp;

import core.app.has_addons_c;

import core.gfx.Camera;

import addons.ResourceManager;

import examples.base.DemoBase;
import examples.base.Renderer;

import demos.virtual_texture.DemoApp;

namespace demo {

export constexpr inline auto run =
    []<core::app::has_addons_c<addons::ResourceManagerTag> App_T>(App_T&& app) -> void {
    app.resource_manager.template get<examples::base::DemoBase>().run(
        [demo_app = std::ref(app.resource_manager.template get<DemoApp>()
         )](examples::base::Renderer& renderer,
            const vk::Extent2D        framebuffer_size,
            core::gfx::Camera         camera) {
            renderer.render(
                framebuffer_size,
                camera,
                std::bind_front(&DemoApp::render, std::ref(demo_app))
            );
        }
    );
};

}   // namespace demo
