#include "SwapchainHolder.hpp"

#include <array>
#include <limits>
#include <ranges>

#include <spdlog/spdlog.h>

namespace core::renderer {

SwapchainHolder::SwapchainHolder(
    vk::UniqueSurfaceKHR&&  t_surface,
    Device&                 t_device,
    FramebufferSizeGetter&& t_get_framebuffer_size
) noexcept
    : m_surface{ std::move(t_surface) },
      m_device{ t_device },
      m_get_framebuffer_size{ std::move(t_get_framebuffer_size) }
{
    recreate_swapchain();
}

auto SwapchainHolder::surface() const noexcept -> vk::SurfaceKHR
{
    return *m_surface;
}

auto SwapchainHolder::get() const noexcept -> const std::optional<Swapchain>&
{
    return m_swapchain;
}

auto SwapchainHolder::set_framebuffer_size(const vk::Extent2D t_framebuffer_size
) noexcept -> void
try {
    if (!m_swapchain.has_value()) {
        recreate_swapchain(t_framebuffer_size);
        return;
    }

    if (const auto extent = Swapchain::choose_extent(
            t_framebuffer_size,
            m_device.get().physical_device().getSurfaceCapabilitiesKHR(*m_surface)
        );
        m_swapchain->extent() != extent)
    {
        recreate_swapchain(t_framebuffer_size);
    }
} catch (const vk::Error& t_error) {
    SPDLOG_ERROR(t_error.what());
}

auto SwapchainHolder::acquire_next_image(
    const vk::Semaphore t_semaphore,
    const vk::Fence     t_fence
) -> std::optional<uint32_t>
try {
    if (m_swapchain.has_value()) {
        switch (const auto [result, image_index]{ m_device.get()->acquireNextImageKHR(
            **m_swapchain, std::numeric_limits<uint64_t>::max(), t_semaphore, t_fence
        ) };
                result)
        {
            case vk::Result::eSuccess:
            case vk::Result::eSuboptimalKHR: {
                m_image_index = image_index;
                return image_index;
            }
            default: {
                SPDLOG_ERROR(
                    "vk::Device::acquireNextImage succeeded with result: {}",
                    std::to_underlying(result)
                );
            }
        }
    }

    recreate_swapchain();
    return std::nullopt;
} catch (const vk::OutOfDateKHRError&) {
    recreate_swapchain();
    return std::nullopt;
}

auto SwapchainHolder::present(std::span<vk::Semaphore> t_wait_semaphores) -> void
try {
    const std::array<vk::SwapchainKHR, 1> swapchains{ **m_swapchain };
    const vk::PresentInfoKHR              info{
                     .waitSemaphoreCount = static_cast<uint32_t>(t_wait_semaphores.size()),
                     .pWaitSemaphores = t_wait_semaphores.data(),
                     .swapchainCount  = static_cast<uint32_t>(swapchains.size()),
                     .pSwapchains     = swapchains.data(),
                     .pImageIndices   = &m_image_index
    };

    if (const auto result{
            static_cast<vk::Queue>(
                m_device.get().info().get_queue(vkb::QueueType::present).value()
            )
                .presentKHR(info) };
        result == vk::Result::eSuboptimalKHR)
    {
        recreate_swapchain();
    }
} catch (const vk::OutOfDateKHRError&) {
    recreate_swapchain();
}

auto SwapchainHolder::on_swapchain_recreated(
    SwapchainHolder::SwapchainRecreatedEvent&& t_swapchain_recreated_event
) -> uint32_t
{
    m_swapchain_recreated_events.emplace_back(
        m_swapchain_recreated_events_counter, std::move(t_swapchain_recreated_event)
    );
    return m_swapchain_recreated_events_counter++;
}

auto SwapchainHolder::remove_swapchain_recreated_event(uint32_t t_id) noexcept -> void
{
    std::erase_if(m_swapchain_recreated_events, [t_id](const auto& t_pair) {
        return t_pair.first == t_id;
    });
}

auto SwapchainHolder::recreate_swapchain(const vk::Extent2D t_framebuffer_size) -> void
{
    m_device.get()->waitIdle();

    auto new_swapchain{ Swapchain::create(
        *m_surface,
        m_device.get().physical_device(),
        m_device.get().info().get_queue_index(vkb::QueueType::graphics).value(),
        m_device.get().info().get_queue_index(vkb::QueueType::present).value(),
        *m_device.get(),
        t_framebuffer_size,
        m_swapchain.transform(&Swapchain::operator*).value_or(nullptr)
    ) };
    m_swapchain.reset();
    m_swapchain = std::move(new_swapchain);

    if (m_swapchain.has_value()) {
        for (const auto& event : m_swapchain_recreated_events | std::views::values) {
            std::invoke(event, m_swapchain.value());
        }
    }
}

auto SwapchainHolder::recreate_swapchain() noexcept -> void
{
    if (m_get_framebuffer_size) {
        recreate_swapchain(m_get_framebuffer_size());
    }
}

}   // namespace core::renderer
