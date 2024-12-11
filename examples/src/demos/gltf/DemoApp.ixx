module;

#include <filesystem>

#include <vulkan/vulkan.hpp>

export module demos.gltf.DemoApp;

import core.app.App;

import core.cache.Cache;

import core.gfx.Camera;

import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.base.swapchain.SwapchainHolder;
import core.renderer.resources.Image;
import core.renderer.scene.Scene;

namespace demo {

export class DemoApp;

export struct DemoPlugin {
    std::reference_wrapper<const std::filesystem::path> model_filepath;
    bool                                                use_virtual_images{};

    [[nodiscard]]
    auto operator()(
        core::cache::Cache&                    cache,
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

export class DemoApp {
public:
    DemoApp(
        core::cache::Cache&                    cache,
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder,
        const std::filesystem::path&           model_filepath,
        bool                                   use_virtual_images
    );

    auto record_command_buffer(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const core::gfx::Camera& camera
    ) -> void;

private:
    std::reference_wrapper<const core::renderer::base::Allocator> m_allocator;
    std::reference_wrapper<const core::renderer::base::Device>    m_device;
    vk::UniqueRenderPass                                          m_render_pass;
    core::renderer::resources::Image                              m_depth_image;
    vk::UniqueImageView                                           m_depth_image_view;
    std::vector<vk::UniqueFramebuffer>                            m_framebuffers;
    core::renderer::Scene                                         m_scene;
};

}   // namespace demo
