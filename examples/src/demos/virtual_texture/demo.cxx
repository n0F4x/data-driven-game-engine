module;

#include <functional>

#include <vulkan/vulkan.hpp>

module demos.virtual_texture;

import core.app.App;

import core.gfx.Camera;

import examples.base.DemoBase;
import examples.base.Renderer;

import demos.virtual_texture.DemoApp;

auto demo::run(App app) -> void
{
    app.resources.at<examples::base::DemoBase>().run(
        [demo_app = std::ref(app.resources.at<DemoApp>()
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
}
