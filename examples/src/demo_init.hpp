#pragma once

#include <vector>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <engine/graphics/Model.hpp>
#include <engine/renderer/Device.hpp>
#include <engine/utility/vulkan/raii_wrappers.hpp>
#include <engine/utility/vulkan/VmaImage.hpp>

namespace init {

[[nodiscard]] auto create_render_pass(
    const vk::SurfaceFormatKHR&     t_surface_format,
    const engine::renderer::Device& t_device
) noexcept -> engine::vulkan::RenderPass;

[[nodiscard]] auto create_depth_image(
    const engine::renderer::Device& t_device,
    vk::Extent2D                    t_swap_chain_extent
) noexcept -> engine::vulkan::VmaImage;

[[nodiscard]] auto create_depth_image_view(
    const engine::renderer::Device& t_device,
    vk::Image                       t_depth_image
) noexcept -> engine::vulkan::ImageView;

[[nodiscard]] auto create_framebuffers(
    vk::Device                        t_device,
    vk::Extent2D                      t_swapchain_extent,
    const std::vector<vk::ImageView>& t_swapchain_image_views,
    vk::RenderPass                    t_render_pass,
    vk::ImageView                     t_depth_image_view
) noexcept -> std::vector<engine::vulkan::Framebuffer>;

[[nodiscard]] auto create_descriptor_set_layout(vk::Device t_device) noexcept
    -> engine::vulkan::DescriptorSetLayout;

[[nodiscard]] auto create_pipeline_layout(
    vk::Device              t_device,
    vk::DescriptorSetLayout t_descriptor_set_layout,
    uint32_t                t_push_constant_size
) -> engine::vulkan::PipelineLayout;

[[nodiscard]] auto create_pipeline(
    vk::Device         t_device,
    vk::PipelineLayout t_pipeline_layout,
    vk::RenderPass     t_render_pass
) -> engine::vulkan::Pipeline;

[[nodiscard]] auto create_command_pool(
    vk::Device t_device,
    uint32_t   t_queue_family_index
) noexcept -> engine::vulkan::CommandPool;

[[nodiscard]] auto create_command_buffers(
    vk::Device      t_device,
    vk::CommandPool t_command_pool,
    uint32_t        t_count
) noexcept -> std::vector<vk::CommandBuffer>;

[[nodiscard]] auto create_descriptor_pool(
    vk::Device t_device,
    uint32_t   t_count
) noexcept -> engine::vulkan::DescriptorPool;

[[nodiscard]] auto create_semaphores(
    vk::Device t_device,
    uint32_t   t_count
) noexcept -> std::vector<engine::vulkan::Semaphore>;

[[nodiscard]] auto create_fences(vk::Device t_device, uint32_t t_count) noexcept
    -> std::vector<engine::vulkan::Fence>;

[[nodiscard]] auto create_model(const std::string& t_path) noexcept
    -> tl::optional<engine::gfx::Model>;

[[nodiscard]] auto count_meshes(const engine::gfx::Model& t_model) noexcept
    -> uint32_t;

}   // namespace init
