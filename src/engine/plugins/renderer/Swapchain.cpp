#include "Swapchain.hpp"

#include <limits>

#include <spdlog/spdlog.h>

#include "helpers.hpp"

namespace engine::renderer {

//////////////////////////////////!
///-----------------------------///
///  Swapchain   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Swapchain::Swapchain(
    utils::vulkan::Surface&&        t_surface,
    Device&                         t_device,
    std::function<vk::Extent2D()>&& t_get_framebuffer_size
) noexcept
    : m_surface{ std::move(t_surface) },
      m_device{ t_device },
      m_get_framebuffer_size{ std::move(t_get_framebuffer_size) }
{
    recreate_swap_chain();
}

auto Swapchain::surface() const noexcept -> vk::SurfaceKHR
{
    return *m_surface;
}

auto Swapchain::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (!m_swap_chain.has_value()) {
        recreate_swap_chain(t_framebuffer_size);
        return;
    }

    auto surface_capabilities{
        m_device.physical_device().getSurfaceCapabilitiesKHR(*m_surface)
    };
    if (surface_capabilities.result != vk::Result::eSuccess) {
        return;
    }
    auto extent = utils::vulkan::Swapchain::choose_extent(
        t_framebuffer_size, surface_capabilities.value
    );

    if (m_swap_chain->extent() != extent) {
        recreate_swap_chain(t_framebuffer_size);
    }
}

auto Swapchain::acquire_next_image(
    vk::Semaphore t_semaphore,
    vk::Fence     t_fence
) noexcept -> Result
{
    const auto [result, image_index]{ m_device->acquireNextImageKHR(
        **m_swap_chain,
        std::numeric_limits<uint64_t>::max(),
        t_semaphore,
        t_fence
    ) };

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreate_swap_chain();
    }

    if ((result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        || !m_swap_chain.has_value())
    {
        SPDLOG_ERROR(
            "vk::Device::acquireNextImage failed with error code {}",
            std::to_underlying(result)
        );
        return Result::eFailure;
    }

    return Result::eSuccess;
}

auto Swapchain::present(std::span<vk::Semaphore> t_wait_semaphores) noexcept
    -> void
{
    const std::array<vk::SwapchainKHR, 1> swapchains{ **m_swap_chain };
    const vk::PresentInfoKHR              info{
                     .waitSemaphoreCount = static_cast<uint32_t>(t_wait_semaphores.size()),
                     .pWaitSemaphores = t_wait_semaphores.data(),
                     .swapchainCount = static_cast<uint32_t>(swapchains.size()),
                     .pSwapchains    = swapchains.data(),
                     .pImageIndices  = &m_image_index
    };

    const auto result{ m_device.graphics_queue().presentKHR(info) };
    if (result == vk::Result::eErrorOutOfDateKHR
        || result == vk::Result::eSuboptimalKHR)
    {
        recreate_swap_chain();
    }
}

auto Swapchain::swapchain_recreated_sink() noexcept -> SwapchainRecreatedSink&
{
    return m_swapchain_recreated_sink;
}

auto Swapchain::recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (m_device->waitIdle() != vk::Result::eSuccess) {
        return;
    }

    auto new_swap_chain{ utils::vulkan::Swapchain::create(
        *m_surface,
        m_device.physical_device(),
        m_device.graphics_queue_family_index(),
        m_device.graphics_queue_family_index(),
        *m_device,
        t_framebuffer_size,
        m_swap_chain.transform(&utils::vulkan::Swapchain::operator*)
            .value_or(nullptr)
    ) };
    m_swap_chain.reset();
    m_swap_chain = std::move(new_swap_chain);

    if (m_swap_chain.has_value()) {
        m_swapchain_recreated_signal.publish(*m_swap_chain);
    }
}

auto Swapchain::recreate_swap_chain() noexcept -> void
{
    if (m_get_framebuffer_size) {
        recreate_swap_chain(m_get_framebuffer_size());
    }
}

}   // namespace engine::renderer
