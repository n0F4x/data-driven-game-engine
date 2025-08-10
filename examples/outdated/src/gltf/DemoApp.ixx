module;

#include <filesystem>

#include <vulkan/vulkan.hpp>

export module demos.gltf.DemoApp;

import modules.cache.Cache;

import modules.gfx.Camera;

import modules.renderer.base.device.Device;
import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.swapchain.SwapchainHolder;
import modules.renderer.resources.Image;
import modules.renderer.scene.Scene;

namespace demo {

export class DemoApp;

export struct DemoPlugin {
    std::reference_wrapper<const std::filesystem::path> model_filepath;
    bool                                                use_virtual_images{};

    [[nodiscard]]
    auto operator()(
        modules::cache::Cache&                    cache,
        const modules::renderer::base::Device&    device,
        const modules::renderer::base::Allocator& allocator,
        modules::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

export class DemoApp {
public:
    DemoApp(
        modules::cache::Cache&                    cache,
        const modules::renderer::base::Device&    device,
        const modules::renderer::base::Allocator& allocator,
        modules::renderer::base::SwapchainHolder& swapchain_holder,
        const std::filesystem::path&           model_filepath,
        bool                                   use_virtual_images
    );

    auto record_command_buffer(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const modules::gfx::Camera& camera
    ) -> void;

private:
    std::reference_wrapper<const modules::renderer::base::Allocator> m_allocator;
    std::reference_wrapper<const modules::renderer::base::Device>    m_device;
    vk::UniqueRenderPass                                          m_render_pass;
    modules::renderer::resources::Image                              m_depth_image;
    vk::UniqueImageView                                           m_depth_image_view;
    std::vector<vk::UniqueFramebuffer>                            m_framebuffers;
    modules::renderer::Scene                                         m_scene;
};

}   // namespace demo
