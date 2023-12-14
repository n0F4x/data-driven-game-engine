#include "Swapchain.hpp"

#include <limits>

#include <spdlog/spdlog.h>

#include "helpers.hpp"

namespace engine::renderer {

//////////////////////////////////!
///-----------------------------///
///  SwapchainHolder   IMPLEMENTATION  ///
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
    recreate_swapchain();
}

auto Swapchain::surface() const noexcept -> vk::SurfaceKHR
{
    return *m_surface;
}

auto Swapchain::get() const noexcept
    -> const tl::optional<vulkan::SwapchainHolder>&
{
    return m_swapchain;
}

auto Swapchain::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (!m_swapchain.has_value()) {
        recreate_swapchain(t_framebuffer_size);
        return;
    }

    auto surface_capabilities{
        m_device.physical_device().getSurfaceCapabilitiesKHR(*m_surface)
    };
    if (surface_capabilities.result != vk::Result::eSuccess) {
        return;
    }
    auto extent = vulkan::SwapchainHolder::choose_extent(
        t_framebuffer_size, surface_capabilities.value
    );

    if (m_swapchain->extent() != extent) {
        recreate_swapchain(t_framebuffer_size);
    }
}

auto Swapchain::acquire_next_image(
    vk::Semaphore t_semaphore,
    vk::Fence     t_fence
) noexcept -> tl::optional<uint32_t>
{
    if (!m_swapchain.has_value()) {
        return tl::nullopt;
    }

    const auto [result, image_index]{ m_device->acquireNextImageKHR(
        **m_swapchain,
        std::numeric_limits<uint64_t>::max(),
        t_semaphore,
        t_fence
    ) };
    m_image_index = image_index;

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreate_swapchain();
    }

    if (!m_swapchain.has_value()) {
        return tl::nullopt;
    }

    switch (result) {
        case vk::Result::eSuccess:
        case vk::Result::eSuboptimalKHR: return image_index;
        default: {
            SPDLOG_ERROR(
                "vk::Device::acquireNextImage failed with error code {}",
                std::to_underlying(result)
            );
            return tl::nullopt;
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
        recreate_swapchain();
    }
}

auto Swapchain::on_swapchain_recreated(
    Swapchain::SwapchainRecreatedEvent&& t_swapchain_recreated_event
) noexcept -> uint32_t
{
    m_swapchain_recreated_events.emplace_back(
        m_swapchain_recreated_events_counter,
        std::move(t_swapchain_recreated_event)
    );
    return m_swapchain_recreated_events_counter++;
}

auto Swapchain::remove_swapchain_recreated_event(uint32_t t_id) noexcept -> void
{
    std::erase_if(m_swapchain_recreated_events, [t_id](const auto& t_pair) {
        return t_pair.first == t_id;
    });
}

auto Swapchain::recreate_swapchain(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (m_device->waitIdle() != vk::Result::eSuccess) {
        return;
    }

    auto new_swapchain{ vulkan::SwapchainHolder::create(
        *m_surface,
        m_device.physical_device(),
        m_device.graphics_queue_family_index(),
        m_device.graphics_queue_family_index(),
        *m_device,
        t_framebuffer_size,
        m_swapchain.transform(&vulkan::SwapchainHolder::operator*)
            .value_or(nullptr)
    ) };
    m_swapchain.reset();
    m_swapchain = std::move(new_swapchain);

    if (m_swapchain.has_value()) {
        for (const auto& [id, event] : m_swapchain_recreated_events) {
            std::invoke(event, m_swapchain.value());
        }
    }
}

auto Swapchain::recreate_swapchain() noexcept -> void
{
    if (m_get_framebuffer_size) {
        recreate_swapchain(m_get_framebuffer_size());
    }
}

}   // namespace engine::renderer
