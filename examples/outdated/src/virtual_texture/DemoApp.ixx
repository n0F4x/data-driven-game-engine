module;

#include <vulkan/vulkan.hpp>

export module demos.virtual_texture.DemoApp;

import modules.gfx.Camera;
import modules.gfx.resources.VirtualImage;

import modules.renderer.base.device.Device;
import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.descriptor_pool.DescriptorPool;
import modules.renderer.base.swapchain.SwapchainHolder;
import modules.renderer.resources.Buffer;
import modules.renderer.resources.Image;
import modules.renderer.resources.RandomAccessBuffer;

import modules.renderer.DeviceInjection;
import modules.renderer.InstanceInjection;

import demos.virtual_texture.Camera;
import demos.virtual_texture.VirtualTexture;
import demos.virtual_texture.VirtualTextureInfo;

namespace demo {

export class DemoApp;

export struct DemoPlugin {
    static auto setup(
        modules::renderer::InstanceInjection& instance_injection,
        modules::renderer::DeviceInjection&   device_injection
    ) -> void;

    auto operator()(
        const modules::renderer::base::Device&    device,
        const modules::renderer::base::Allocator& allocator,
        modules::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

export class DemoApp {
public:
    DemoApp(
        const modules::renderer::base::Device&    device,
        const modules::renderer::base::Allocator& allocator,
        modules::renderer::base::SwapchainHolder& swapchain_holder
    );

    auto render(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const modules::gfx::Camera& camera
    ) -> void;

private:
    std::reference_wrapper<modules::renderer::base::SwapchainHolder> m_swapchain_holder_ref;

    vk::UniqueDescriptorSetLayout m_descriptor_set_layout;
    vk::UniquePipelineLayout      m_pipeline_layout;

    modules::renderer::resources::Image m_depth_image;
    vk::UniqueImageView              m_depth_image_view;

    modules::renderer::base::DescriptorPool m_descriptor_pool;
    vk::UniquePipeline                   m_debug_texture_pipeline;
    vk::UniquePipeline                   m_virtual_texture_pipeline;

    modules::renderer::resources::RandomAccessBuffer<Camera> m_camera_buffer;

    VirtualTexture m_virtual_texture;

    modules::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>
        m_virtual_texture_info_buffer;

    // This is also a random access buffer, but with runtime size
    modules::renderer::resources::Buffer m_virtual_texture_blocks_buffer;
    modules::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>
        m_virtual_texture_blocks_uniform;

    vk::UniqueDescriptorSet m_virtual_texture_descriptor_set;
    vk::UniqueDescriptorSet m_debug_texture_descriptor_set;

    auto draw(vk::CommandBuffer graphics_command_buffer, const modules::gfx::Camera& camera)
        -> void;

    auto update_virtual_texture(const modules::gfx::Camera& camera) -> void;

    auto draw_debug(vk::CommandBuffer graphics_command_buffer) -> void;
};

}   // namespace demo
