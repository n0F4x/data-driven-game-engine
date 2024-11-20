module;

#include <vulkan/vulkan.hpp>

#include <core/gfx/Camera.hpp>
#include <core/gfx/resources/VirtualImage.hpp>
#include <core/renderer/base/descriptor_pool/DescriptorPool.hpp>
#include <core/renderer/base/swapchain/SwapchainHolder.hpp>
#include <core/renderer/resources/Image.hpp>
#include <core/renderer/resources/RandomAccessBuffer.hpp>
#include <core/renderer/resources/Buffer.hpp>

export module demos.virtual_texture.DemoApp;

import store.StoreView;

import demos.virtual_texture.Camera;
import demos.virtual_texture.VirtualTexture;
import demos.virtual_texture.VirtualTextureInfo;

namespace demo {

export class DemoApp;

export struct DemoPlugin {
    static auto setup(StoreView plugins) -> void;

    auto operator()(
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

export class DemoApp {
public:
    DemoApp(
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder
    );

    auto render(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const core::gfx::Camera& camera
    ) -> void;

private:
    std::reference_wrapper<core::renderer::base::SwapchainHolder> m_swapchain_holder_ref;

    vk::UniqueDescriptorSetLayout m_descriptor_set_layout;
    vk::UniquePipelineLayout      m_pipeline_layout;

    core::renderer::resources::Image m_depth_image;
    vk::UniqueImageView              m_depth_image_view;

    core::renderer::base::DescriptorPool m_descriptor_pool;
    vk::UniquePipeline                   m_debug_texture_pipeline;
    vk::UniquePipeline                   m_virtual_texture_pipeline;

    core::renderer::resources::RandomAccessBuffer<Camera> m_camera_buffer;

    VirtualTexture m_virtual_texture;

    core::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>
        m_virtual_texture_info_buffer;

    // This is also a random access buffer, but with runtime size
    core::renderer::resources::Buffer m_virtual_texture_blocks_buffer;
    core::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>
        m_virtual_texture_blocks_uniform;

    vk::UniqueDescriptorSet m_virtual_texture_descriptor_set;
    vk::UniqueDescriptorSet m_debug_texture_descriptor_set;

    auto draw(vk::CommandBuffer graphics_command_buffer, const core::gfx::Camera& camera)
        -> void;

    auto update_virtual_texture(const core::gfx::Camera& camera) -> void;

    auto draw_debug(vk::CommandBuffer graphics_command_buffer) -> void;
};

}   // namespace demo
