#pragma once

#include <vector>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <engine/gfx/Model.hpp>
#include <engine/renderer/Device.hpp>
#include <engine/utility/vma/Image.hpp>

namespace init {

[[nodiscard]] auto create_render_pass(
    const vk::SurfaceFormatKHR&     t_surface_format,
    const engine::renderer::Device& t_device
) -> vk::UniqueRenderPass;

[[nodiscard]] auto create_depth_image(
    const engine::renderer::Device& t_device,
    vk::Extent2D                    t_swapchain_extent
) noexcept -> engine::vulkan::vma::Image;

[[nodiscard]] auto create_depth_image_view(
    const engine::renderer::Device& t_device,
    vk::Image                       t_depth_image
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

[[nodiscard]] auto create_command_pool(
    vk::Device t_device,
    uint32_t   t_queue_family_index
) -> vk::UniqueCommandPool;

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

[[nodiscard]] auto create_model(const std::string& t_path) noexcept
    -> tl::optional<engine::gfx::Model>;

[[nodiscard]] auto count_meshes(const engine::gfx::Model& t_model) noexcept
    -> uint32_t;

}   // namespace init
