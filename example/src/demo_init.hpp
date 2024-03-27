#pragma once

#include <vector>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <core/renderer/base/Allocator.hpp>
#include <core/utility/vma/Image.hpp>

#include "core/renderer/model/StagingModel.hpp"

namespace init {

[[nodiscard]] auto create_render_pass(
    const vk::SurfaceFormatKHR&   t_surface_format,
    const core::renderer::Device& t_device
) -> vk::UniqueRenderPass;

[[nodiscard]] auto create_depth_image(
    vk::PhysicalDevice t_physical_device,
    VmaAllocator       t_allocator,
    vk::Extent2D       t_swapchain_extent
) noexcept -> core::vma::Image;

[[nodiscard]] auto create_depth_image_view(
    const core::renderer::Device& t_device,
    vk::Image                     t_depth_image
) -> vk::UniqueImageView;

[[nodiscard]] auto create_framebuffers(
    vk::Device                              t_device,
    vk::Extent2D                            t_swapchain_extent,
    const std::vector<vk::UniqueImageView>& t_swapchain_image_views,
    vk::RenderPass                          t_render_pass,
    vk::ImageView                           t_depth_image_view
) -> std::vector<vk::UniqueFramebuffer>;

[[nodiscard]] auto create_descriptor_set_layout(vk::Device t_device)
    -> vk::UniqueDescriptorSetLayout;

[[nodiscard]] auto create_pipeline_layout(
    vk::Device              t_device,
    vk::DescriptorSetLayout t_descriptor_set_layout,
    uint32_t                t_push_constant_size
) -> vk::UniquePipelineLayout;

[[nodiscard]] auto create_pipeline(
    vk::Device         t_device,
    vk::PipelineLayout t_pipeline_layout,
    vk::RenderPass     t_render_pass
) -> vk::UniquePipeline;

[[nodiscard]] auto create_command_pool(vk::Device t_device, uint32_t t_queue_family_index)
    -> vk::UniqueCommandPool;

[[nodiscard]] auto create_command_buffers(
    vk::Device      t_device,
    vk::CommandPool t_command_pool,
    uint32_t        t_count
) -> std::vector<vk::CommandBuffer>;

[[nodiscard]] auto create_descriptor_pool(vk::Device t_device, uint32_t t_count)
    -> vk::UniqueDescriptorPool;

[[nodiscard]] auto create_semaphores(vk::Device t_device, uint32_t t_count)
    -> std::vector<vk::UniqueSemaphore>;

[[nodiscard]] auto create_fences(vk::Device t_device, uint32_t t_count)
    -> std::vector<vk::UniqueFence>;

[[nodiscard]] auto count_meshes(const core::renderer::StagingModel& t_model) noexcept
    -> uint32_t;

auto upload_model(
    const core::renderer::Device&    t_device,
    const core::renderer::Allocator& t_allocator,
    core::renderer::StagingModel&&   t_staging_model,
    vk::DescriptorSetLayout          t_descriptor_set_layout,
    vk::DescriptorPool               t_descriptor_pool
) -> tl::optional<core::renderer::RenderModel>;

}   // namespace init
