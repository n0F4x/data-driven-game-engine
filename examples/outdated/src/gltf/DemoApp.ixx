module;

#include <filesystem>

#include <vulkan/vulkan.hpp>

export module demos.gltf.DemoApp;

import ddge.modules.cache.Cache;

import ddge.modules.gfx.Camera;

import ddge.modules.renderer.base.device.Device;
import ddge.modules.renderer.base.allocator.Allocator;
import ddge.modules.renderer.base.swapchain.SwapchainHolder;
import ddge.modules.renderer.resources.Image;
import ddge.modules.renderer.scene.Scene;

namespace demo {

export class DemoApp;

export struct DemoPlugin {
    std::reference_wrapper<const std::filesystem::path> model_filepath;
    bool                                                use_virtual_images{};

    [[nodiscard]]
    auto operator()(
        ddge::cache::Cache&                    cache,
        const ddge::renderer::base::Device&    device,
        const ddge::renderer::base::Allocator& allocator,
        ddge::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

export class DemoApp {
public:
    DemoApp(
        ddge::cache::Cache&                    cache,
        const ddge::renderer::base::Device&    device,
        const ddge::renderer::base::Allocator& allocator,
        ddge::renderer::base::SwapchainHolder& swapchain_holder,
        const std::filesystem::path&           model_filepath,
        bool                                   use_virtual_images
    );

    auto record_command_buffer(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const ddge::gfx::Camera& camera
    ) -> void;

private:
    std::reference_wrapper<const ddge::renderer::base::Allocator> m_allocator;
    std::reference_wrapper<const ddge::renderer::base::Device>    m_device;
    vk::UniqueRenderPass                                          m_render_pass;
    ddge::renderer::resources::Image                              m_depth_image;
    vk::UniqueImageView                                           m_depth_image_view;
    std::vector<vk::UniqueFramebuffer>                            m_framebuffers;
    ddge::renderer::Scene                                         m_scene;
};

}   // namespace demo
