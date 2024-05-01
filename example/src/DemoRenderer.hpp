#pragma once

#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/renderer/base/device/Device.hpp>
#include <core/renderer/base/swapchain/Swapchain.hpp>
#include <core/renderer/memory/Image.hpp>
#include <core/renderer/scene/Scene.hpp>

struct DemoRenderer {
    core::renderer::Device&            device;
    core::renderer::Allocator&         allocator;
    core::renderer::Swapchain&         swapchain;
    vk::UniqueRenderPass               render_pass;
    core::renderer::Image              depth_image;
    vk::UniqueImageView                depth_image_view;
    std::vector<vk::UniqueFramebuffer> framebuffers;
    vk::UniqueCommandPool              command_pool;
    std::vector<vk::CommandBuffer>     command_buffers;
    std::vector<vk::UniqueSemaphore>   image_acquired_semaphores;
    std::vector<vk::UniqueSemaphore>   render_finished_semaphores;
    std::vector<vk::UniqueFence>       in_flight_fences;
    uint32_t                           frame_index{};
    core::renderer::Scene              scene;

    [[nodiscard]]
    static auto create(Store& t_store, const std::string& t_model_filepath)
        -> tl::optional<DemoRenderer>;

    auto render(
        vk::Extent2D                  t_framebuffer_size,
        const core::graphics::Camera& t_camera
    ) noexcept -> void;

private:
    auto record_command_buffer(
        uint32_t               t_image_index,
        core::graphics::Camera t_camera
    ) noexcept -> void;
};
