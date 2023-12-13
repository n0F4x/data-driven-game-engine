#pragma once

#include <expected>
#include <functional>
#include <utility>
#include <variant>
#include <vector>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <entt/signal/sigh.hpp>

#include "engine/utils/vulkan/raii_wrappers.hpp"
#include "engine/utils/vulkan/result_types.hpp"
#include "engine/utils/vulkan/Swapchain.hpp"

#include "Device.hpp"

namespace engine::renderer {

namespace err {

/**
 * No swapchain is present within Swapchain.
 */
class NoSwapchain {};

}   // namespace err

class Swapchain {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using FramebufferSizeGetter = std::function<vk::Extent2D()>;
    using SwapchainRecreatedEvent =
        std::function<void(const vulkan::Swapchain&)>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Swapchain(
        vulkan::Surface&&       t_surface,
        Device&                 t_device,
        FramebufferSizeGetter&& t_get_framebuffer_size
    ) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto surface() const noexcept -> vk::SurfaceKHR;
    [[nodiscard]] auto get() const noexcept
        -> const tl::optional<vulkan::Swapchain>&;

    auto set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept -> void;

    [[nodiscard]] auto acquire_next_image(
        vk::Semaphore t_semaphore = nullptr,
        vk::Fence     t_fence     = nullptr
    ) noexcept -> tl::optional<uint32_t>;

    auto present(std::span<vk::Semaphore> t_wait_semaphores = {}) noexcept
        -> void;

    auto on_swapchain_recreated(
        SwapchainRecreatedEvent&& t_swapchain_recreated_event
    ) noexcept -> uint32_t;

    auto remove_swapchain_recreated_event(uint32_t t_id) noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vulkan::Surface                 m_surface;
    Device&                         m_device;
    FramebufferSizeGetter           m_get_framebuffer_size;
    tl::optional<vulkan::Swapchain> m_swapchain;
    uint32_t                        m_image_index{};
    std::vector<std::pair<uint32_t, SwapchainRecreatedEvent>>
             m_swapchain_recreated_events;
    uint32_t m_swapchain_recreated_events_counter{};

    ///***********///
    ///  Methods  ///
    ///***********///
    auto recreate_swapchain(vk::Extent2D t_framebuffer_size) noexcept -> void;
    auto recreate_swapchain() noexcept -> void;
};

}   // namespace engine::renderer
