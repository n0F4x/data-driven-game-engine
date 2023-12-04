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
    vulkan::Surface&&       t_surface,
    Device&                 t_device,
    FramebufferSizeGetter&& t_get_framebuffer_size
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

auto Swapchain::get() const noexcept -> const tl::optional<vulkan::Swapchain>&
{
    return m_swapchain;
}

auto Swapchain::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (!m_swapchain.has_value()) {
        recreate_swap_chain(t_framebuffer_size);
        return;
    }

    auto surface_capabilities{
        m_device.physical_device().getSurfaceCapabilitiesKHR(*m_surface)
    };
    if (surface_capabilities.result != vk::Result::eSuccess) {
        return;
    }
    auto extent = vulkan::Swapchain::choose_extent(
        t_framebuffer_size, surface_capabilities.value
    );

    if (m_swapchain->extent() != extent) {
        recreate_swap_chain(t_framebuffer_size);
    }
}

auto Swapchain::acquire_next_image(
    vk::Semaphore t_semaphore,
    vk::Fence     t_fence
) noexcept
    -> std::expected<
        std::variant<vulkan::err::Success, vulkan::err::SuboptimalKHR>,
        std::variant<
            err::NoSwapchain,
            vulkan::err::ErrorOutOfHostMemory,
            vulkan::err::ErrorOutOfDeviceMemory,
            vulkan::err::ErrorDeviceLost,
            vulkan::err::ErrorSurfaceLostKHR,
            vulkan::err::ErrorFullScreenExclusiveModeLostEXT>>
{
    const auto [result, image_index]{ m_device->acquireNextImageKHR(
        **m_swapchain,
        std::numeric_limits<uint64_t>::max(),
        t_semaphore,
        t_fence
    ) };

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreate_swap_chain();
    }

    if (!m_swapchain.has_value()) {
        return std::unexpected{ err::NoSwapchain{} };
    }

    switch (result) {
        case vk::Result::eSuccess:
        case vk::Result::eSuboptimalKHR:
            return vulkan::
                get<vulkan::err::Success, vulkan::err::SuboptimalKHR>(result);
        default: {
            SPDLOG_ERROR(
                "vk::Device::acquireNextImage failed with error code {}",
                std::to_underlying(result)
            );
            return std::unexpected{ vulkan::get<
                err::NoSwapchain,
                vulkan::err::ErrorOutOfHostMemory,
                vulkan::err::ErrorOutOfDeviceMemory,
                vulkan::err::ErrorDeviceLost,
                vulkan::err::ErrorSurfaceLostKHR,
                vulkan::err::ErrorFullScreenExclusiveModeLostEXT>(result) };
        }
    }
}

auto Swapchain::present(std::span<vk::Semaphore> t_wait_semaphores) noexcept
    -> void
{
    const std::array<vk::SwapchainKHR, 1> swapchains{ **m_swapchain };
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

    auto new_swap_chain{ vulkan::Swapchain::create(
        *m_surface,
        m_device.physical_device(),
        m_device.graphics_queue_family_index(),
        m_device.graphics_queue_family_index(),
        *m_device,
        t_framebuffer_size,
        m_swapchain.transform(&vulkan::Swapchain::operator*).value_or(nullptr)
    ) };
    m_swapchain.reset();
    m_swapchain = std::move(new_swap_chain);

    if (m_swapchain.has_value()) {
        m_swapchain_recreated_signal.publish(*m_swapchain);
    }
}

auto Swapchain::recreate_swap_chain() noexcept -> void
{
    if (m_get_framebuffer_size) {
        recreate_swap_chain(m_get_framebuffer_size());
    }
}

}   // namespace engine::renderer
