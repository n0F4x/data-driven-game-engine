#pragma once

#include <vulkan/vulkan.hpp>

#include <core/gfx/Camera.hpp>
#include <core/gfx/resources/VirtualImage.hpp>
#include <core/renderer/base/descriptor_pool/DescriptorPool.hpp>
#include <core/renderer/base/swapchain/SwapchainHolder.hpp>
#include <core/renderer/resources/Image.hpp>
#include <core/renderer/resources/RandomAccessBuffer.hpp>
#include <core/store/StoreView.hpp>

#include "Camera.hpp"
#include "VirtualTexture.hpp"

namespace demo {

class DemoApp;

struct DemoPlugin {
    static auto setup(StoreView plugins) -> void;

    auto operator()(
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

class DemoApp {
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
    vk::UniquePipeline                   m_pipeline;

    core::renderer::resources::RandomAccessBuffer<Camera> m_camera_buffer;

    VirtualTexture m_virtual_texture;

    vk::UniqueDescriptorSet m_descriptor_set;

    auto draw(vk::CommandBuffer graphics_command_buffer, const core::gfx::Camera& camera)
        -> void;
};

}   // namespace demo
