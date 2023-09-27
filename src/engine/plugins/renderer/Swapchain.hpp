#pragma once

#include <functional>
#include <optional>

#include <vulkan/vulkan.hpp>

#include <entt/signal/sigh.hpp>

#include "engine/utility/Result.hpp"
#include "engine/utility/vulkan/Surface.hpp"
#include "engine/utility/vulkan/Swapchain.hpp"

#include "Device.hpp"

namespace engine::renderer {

class Swapchain {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using SwapchainRecreatedSigh =
        entt::sigh<void(const utils::vulkan::Swapchain&)>;
    using SwapchainRecreatedSink = entt::sink<SwapchainRecreatedSigh>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Swapchain(
        utils::vulkan::Surface&&        t_surface,
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
        -> SwapchainRecreatedSink&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    utils::vulkan::Surface                  m_surface;
    Device&                                 m_device;
    std::function<vk::Extent2D()>           m_get_framebuffer_size;
    std::optional<utils::vulkan::Swapchain> m_swap_chain;
    uint32_t                                m_image_index{};
    SwapchainRecreatedSigh                  m_swapchain_recreated_signal;
    SwapchainRecreatedSink                  m_swapchain_recreated_sink{
        m_swapchain_recreated_signal
    };

    ///***********///
    ///  Methods  ///
    ///***********///
    auto recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept -> void;
    auto recreate_swap_chain() noexcept -> void;
};

}   // namespace engine::renderer
