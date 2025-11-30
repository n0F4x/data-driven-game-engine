module;

#include <vulkan/vulkan.hpp>

export module demos.virtual_texture.DemoApp;

import ddge.deprecated.gfx.Camera;
import ddge.deprecated.gfx.resources.VirtualImage;

import ddge.deprecated.renderer.base.device.Device;
import ddge.deprecated.renderer.base.allocator.Allocator;
import ddge.deprecated.renderer.base.descriptor_pool.DescriptorPool;
import ddge.deprecated.renderer.base.swapchain.SwapchainHolder;
import ddge.deprecated.renderer.resources.Buffer;
import ddge.deprecated.renderer.resources.Image;
import ddge.deprecated.renderer.resources.RandomAccessBuffer;

import ddge.deprecated.renderer.DeviceInjection;
import ddge.deprecated.renderer.InstanceInjection;

import demos.virtual_texture.Camera;
import demos.virtual_texture.VirtualTexture;
import demos.virtual_texture.VirtualTextureInfo;

namespace demo {

export class DemoApp;

export struct DemoPlugin {
    static auto setup(
        ddge::renderer::InstanceInjection& instance_injection,
        ddge::renderer::DeviceInjection&   device_injection
    ) -> void;

    auto operator()(
        const ddge::renderer::base::Device&    device,
        const ddge::renderer::base::Allocator& allocator,
        ddge::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

export class DemoApp {
public:
    DemoApp(
        const ddge::renderer::base::Device&    device,
        const ddge::renderer::base::Allocator& allocator,
        ddge::renderer::base::SwapchainHolder& swapchain_holder
    );

    auto render(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const ddge::gfx::Camera& camera
    ) -> void;

private:
    std::reference_wrapper<ddge::renderer::base::SwapchainHolder> m_swapchain_holder_ref;

    vk::UniqueDescriptorSetLayout m_descriptor_set_layout;
    vk::UniquePipelineLayout      m_pipeline_layout;

    ddge::renderer::resources::Image m_depth_image;
    vk::UniqueImageView              m_depth_image_view;

    ddge::renderer::base::DescriptorPool m_descriptor_pool;
    vk::UniquePipeline                   m_debug_texture_pipeline;
    vk::UniquePipeline                   m_virtual_texture_pipeline;

    ddge::renderer::resources::RandomAccessBuffer<Camera> m_camera_buffer;

    VirtualTexture m_virtual_texture;

    ddge::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>
        m_virtual_texture_info_buffer;

    // This is also a random access buffer, but with runtime size
    ddge::renderer::resources::Buffer m_virtual_texture_blocks_buffer;
    ddge::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>
        m_virtual_texture_blocks_uniform;

    vk::UniqueDescriptorSet m_virtual_texture_descriptor_set;
    vk::UniqueDescriptorSet m_debug_texture_descriptor_set;

    auto draw(vk::CommandBuffer graphics_command_buffer, const ddge::gfx::Camera& camera)
        -> void;

    auto update_virtual_texture(const ddge::gfx::Camera& camera) -> void;

    auto draw_debug(vk::CommandBuffer graphics_command_buffer) -> void;
};

}   // namespace demo
