#pragma once

#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "core/renderer/base/device/Device.hpp"

#include "Swapchain.hpp"

namespace core::renderer {

class SwapchainHolder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using FramebufferSizeGetter   = std::function<vk::Extent2D()>;
    using SwapchainRecreatedEvent = std::function<void(const Swapchain&)>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit SwapchainHolder(
        vk::UniqueSurfaceKHR&&  t_surface,
        Device&                 t_device,
        FramebufferSizeGetter&& t_get_framebuffer_size = {}
    );

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto surface() const noexcept -> vk::SurfaceKHR;
    [[nodiscard]]
    auto get() const noexcept -> const std::optional<Swapchain>&;

    auto set_framebuffer_size(vk::Extent2D t_framebuffer_size) -> void;

    [[nodiscard]]
    auto acquire_next_image(
        vk::Semaphore t_semaphore = nullptr,
        vk::Fence     t_fence     = nullptr
    ) -> std::optional<uint32_t>;

    auto present(std::span<const vk::Semaphore> t_wait_semaphores = {}) -> void;
    auto present(vk::Semaphore wait_semaphore) -> void;

    auto on_swapchain_recreated(SwapchainRecreatedEvent&& t_swapchain_recreated_event
    ) -> uint32_t;

    auto remove_swapchain_recreated_event(uint32_t t_id) noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::UniqueSurfaceKHR           m_surface;
    std::reference_wrapper<Device> m_device;
    FramebufferSizeGetter          m_get_framebuffer_size;
    std::optional<Swapchain>       m_swapchain;
    uint32_t                       m_image_index{};
    std::vector<std::pair<uint32_t, SwapchainRecreatedEvent>> m_swapchain_recreated_events;
    uint32_t m_swapchain_recreated_events_counter{};

    ///***********///
    ///  Methods  ///
    ///***********///
    auto recreate_swapchain(vk::Extent2D t_framebuffer_size) -> void;
    auto recreate_swapchain() -> void;
};

}   // namespace core::renderer
