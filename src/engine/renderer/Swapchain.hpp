#pragma once

#include <functional>
#include <optional>

#include <vulkan/vulkan.hpp>

#include <entt/signal/sigh.hpp>

#include "engine/utility/Result.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/SwapChain.hpp"

#include "Device.hpp"

namespace engine::renderer {

class Swapchain {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Swapchain(
        vulkan::Surface&&               t_surface,
        Device&                         t_device,
        std::function<vk::Extent2D()>&& t_get_framebuffer_size
    ) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto surface() const noexcept -> vk::SurfaceKHR;
    auto set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept -> void;

    [[nodiscard]] auto acquire_next_image(
        vk::Semaphore t_semaphore = nullptr,
        vk::Fence     t_fence     = nullptr
    ) noexcept -> Result;

    auto present(std::span<vk::Semaphore> t_wait_semaphores = {}) noexcept
        -> void;

    [[nodiscard]] auto swapchain_recreated_sink() noexcept
        -> entt::sink<entt::sigh<void(const vulkan::SwapChain&)>>&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vulkan::Surface                            m_surface;
    Device&                                    m_device;
    std::function<vk::Extent2D()>              m_get_framebuffer_size;
    std::optional<vulkan::SwapChain>           m_swap_chain;
    uint32_t                                   m_image_index{};
    entt::sigh<void(const vulkan::SwapChain&)> m_swapchain_recreated_signal;
    entt::sink<entt::sigh<void(const vulkan::SwapChain&)>>
        m_swapchain_recreated_sink{ m_swapchain_recreated_signal };

    ///***********///
    ///  Methods  ///
    ///***********///
    auto recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept -> void;
    auto recreate_swap_chain() noexcept -> void;
};

}   // namespace engine::renderer
