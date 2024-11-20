module;

#include <functional>

#include <vulkan/vulkan.hpp>

#include <core/gfx/Camera.hpp>

module demos.gltf;

import examples.base.DemoBase;
import examples.base.Renderer;

import demos.gltf.DemoApp;

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
                std::bind_front(&DemoApp::record_command_buffer, std::ref(demo_app))
            );
        }
    );
}
