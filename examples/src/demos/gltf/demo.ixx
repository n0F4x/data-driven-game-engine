module;

#include <functional>

export module demos.gltf;

import vulkan_hpp;

import core.app.App;

import core.gfx.Camera;

import addons.ResourceManager;

import examples.base.DemoBase;
import examples.base.Renderer;

import demos.gltf.DemoApp;

namespace demo {

export constexpr auto run =
    []<core::app::has_addons_c<addons::ResourceManagerTag> App_T>(App_T&& app) -> void {
    std::get<examples::base::DemoBase>(app.resources)
        .run([demo_app = std::ref(std::get<DemoApp>(app.resources))](
                 examples::base::Renderer& renderer,
                 const vk::Extent2D        framebuffer_size,
                 core::gfx::Camera         camera
             ) {
            renderer.render(
                framebuffer_size,
                camera,
                std::bind_front(&DemoApp::record_command_buffer, std::ref(demo_app))
            );
        });
};

}   // namespace demo
