module;

#include <functional>

export module demos.gltf;

import vulkan_hpp;

import core.app.App;

import core.gfx.Camera;

import addons.store.Mixin;

import examples.base.DemoBase;
import examples.base.Renderer;

import demos.gltf.DemoApp;

namespace demo {

export constexpr auto run =
    []<core::app::mixed_with_c<plugins::store::Mixin> App_T>(App_T&& app) -> void {
    app.resources.template at<examples::base::DemoBase>().run(
        [demo_app = std::ref(app.resources.template at<DemoApp>()
         )](examples::base::Renderer& renderer,
            const vk::Extent2D        framebuffer_size,
            core::gfx::Camera         camera) {
            renderer.render(
                framebuffer_size,
                camera,
                std::bind_front(&DemoApp::record_command_buffer, std::ref(demo_app))
            );
        }
    );
};

}   // namespace demo
