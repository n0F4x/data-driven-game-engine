#pragma once

#include <app/App.hpp>
#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/renderer/base/device/Device.hpp>
#include <core/renderer/base/swapchain/SwapchainHolder.hpp>
#include <core/renderer/scene/Scene.hpp>

namespace demo {

class DemoApp;

struct DemoPlugin {
    std::reference_wrapper<const std::filesystem::path> model_filepath;

    [[nodiscard]]
    auto operator()(
        core::cache::Cache&                    cache,
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoApp;
};

class DemoApp {
public:
    DemoApp(
        core::cache::Cache&                    cache,
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator,
        core::renderer::base::SwapchainHolder& swapchain_holder,
        const std::filesystem::path&           model_filepath
    );

    auto record_command_buffer(
        uint32_t                 image_index,
        vk::Extent2D             swapchain_extent,
        vk::CommandBuffer        graphics_command_buffer,
        const core::gfx::Camera& camera
    ) -> void;

private:
    vk::UniqueRenderPass               m_render_pass;
    core::renderer::resources::Image   m_depth_image;
    vk::UniqueImageView                m_depth_image_view;
    std::vector<vk::UniqueFramebuffer> m_framebuffers;
    core::renderer::Scene              m_scene;
};

}   // namespace demo