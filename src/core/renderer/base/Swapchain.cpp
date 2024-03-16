#include "Swapchain.hpp"

#include <array>
#include <limits>

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include "helpers.hpp"

namespace core::renderer {

//////////////////////////////////!
///-----------------------------///
///  Swapchain   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

auto Swapchain::required_instance_extensions() noexcept -> std::span<const std::string>
{
    static const std::vector<std::string> s_extensions_names{
        []() -> std::vector<std::string> {
            if (!glfwInit()) {
                return {};
            }

            uint32_t count;
            const char** glfw_extension_names{ glfwGetRequiredInstanceExtensions(&count) };
            if (glfw_extension_names == nullptr) {
                return {};
            }

            std::vector<std::string> result{};
            result.reserve(count);

            for (uint32_t i{}; i < count; i++) {
                result.emplace_back(glfw_extension_names[i]);
            }

            return result;
        }()
    };

    return s_extensions_names;
}

auto Swapchain::required_device_extensions() noexcept -> std::span<const std::string>
{
    static const std::array<std::string, 1> s_extensions_names{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    return s_extensions_names;
}

Swapchain::Swapchain(
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

auto Swapchain::surface() const noexcept -> vk::SurfaceKHR
{
    return *m_surface;
}

auto Swapchain::get() const noexcept -> const tl::optional<vulkan::Swapchain>&
{
    return m_swapchain;
}

auto Swapchain::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept -> void
try {
    if (!m_swapchain.has_value()) {
        recreate_swapchain(t_framebuffer_size);
        return;
    }

    auto extent = vulkan::Swapchain::choose_extent(
        t_framebuffer_size,
        m_device.get().physical_device().getSurfaceCapabilitiesKHR(*m_surface)
    );

    if (m_swapchain->extent() != extent) {
        recreate_swapchain(t_framebuffer_size);
    }
} catch (const vk::Error& t_error) {
    SPDLOG_ERROR(t_error.what());
}

auto Swapchain::acquire_next_image(vk::Semaphore t_semaphore, vk::Fence t_fence)
    -> tl::optional<uint32_t>
try {
    if (m_swapchain.has_value()) {
        const auto [result, image_index]{ m_device.get()->acquireNextImageKHR(
            **m_swapchain, std::numeric_limits<uint64_t>::max(), t_semaphore, t_fence
        ) };

        switch (result) {
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
    return tl::nullopt;
} catch (const vk::OutOfDateKHRError&) {
    recreate_swapchain();
    return tl::nullopt;
}

auto Swapchain::present(std::span<vk::Semaphore> t_wait_semaphores) -> void
try {
    const std::array<vk::SwapchainKHR, 1> swapchains{ **m_swapchain };
    const vk::PresentInfoKHR              info{
                     .waitSemaphoreCount = static_cast<uint32_t>(t_wait_semaphores.size()),
                     .pWaitSemaphores = t_wait_semaphores.data(),
                     .swapchainCount  = static_cast<uint32_t>(swapchains.size()),
                     .pSwapchains     = swapchains.data(),
                     .pImageIndices   = &m_image_index
    };

    const auto result{ m_device.get().graphics_queue().presentKHR(info) };
    if (result == vk::Result::eSuboptimalKHR) {
        recreate_swapchain();
    }
} catch (const vk::OutOfDateKHRError&) {
    recreate_swapchain();
}

auto Swapchain::on_swapchain_recreated(
    Swapchain::SwapchainRecreatedEvent&& t_swapchain_recreated_event
) noexcept -> uint32_t
{
    m_swapchain_recreated_events.emplace_back(
        m_swapchain_recreated_events_counter, std::move(t_swapchain_recreated_event)
    );
    return m_swapchain_recreated_events_counter++;
}

auto Swapchain::remove_swapchain_recreated_event(uint32_t t_id) noexcept -> void
{
    std::erase_if(m_swapchain_recreated_events, [t_id](const auto& t_pair) {
        return t_pair.first == t_id;
    });
}

auto Swapchain::recreate_swapchain(vk::Extent2D t_framebuffer_size) noexcept -> void
{
    m_device.get()->waitIdle();

    auto new_swapchain{ vulkan::Swapchain::create(
        *m_surface,
        m_device.get().physical_device(),
        m_device.get().graphics_queue_family_index(),
        m_device.get().graphics_queue_family_index(),
        *m_device.get(),
        t_framebuffer_size,
        m_swapchain.transform(&vulkan::Swapchain::operator*).value_or(nullptr)
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

}   // namespace core::renderer
