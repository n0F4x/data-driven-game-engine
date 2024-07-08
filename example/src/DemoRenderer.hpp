#pragma once

#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/renderer/base/device/Device.hpp>
#include <core/renderer/memory/Image.hpp>
#include <core/renderer/scene/Scene.hpp>

#include "core/renderer/base/swapchain/SwapchainHolder.hpp"

struct DemoRenderer {
    std::reference_wrapper<const core::renderer::Device>    device;
    std::reference_wrapper<const core::renderer::Allocator> allocator;
    std::reference_wrapper<core::renderer::SwapchainHolder> swapchain;
    vk::UniqueRenderPass                                    render_pass;
    core::renderer::Image                                   depth_image;
    vk::UniqueImageView                                     depth_image_view;
    std::vector<vk::UniqueFramebuffer>                      framebuffers;
    vk::UniqueCommandPool                                   command_pool;
    std::vector<vk::CommandBuffer>                          command_buffers;
    std::vector<vk::UniqueSemaphore>                        image_acquired_semaphores;
    std::vector<vk::UniqueSemaphore>                        render_finished_semaphores;
    std::vector<vk::UniqueFence>                            render_finished_fences;
    uint32_t                                                frame_index{};
    core::renderer::Scene                                   scene;

    [[nodiscard]]
    static auto create(
        Store&                       t_store,
        const std::filesystem::path& t_model_filepath,
        const std::filesystem::path& t_fragment_shader_filepath
    ) -> std::optional<DemoRenderer>;

    auto render(vk::Extent2D t_framebuffer_size, const core::graphics::Camera& t_camera)
        -> void;

private:
    auto record_command_buffer(
        const core::renderer::Swapchain& t_swapchain,
        uint32_t                         t_image_index,
        core::graphics::Camera           t_camera
    ) -> void;
};
