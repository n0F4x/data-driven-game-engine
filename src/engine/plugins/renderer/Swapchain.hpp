#pragma once

#include <expected>
#include <functional>
#include <variant>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <entt/signal/sigh.hpp>

#include "engine/utility/Result.hpp"
#include "engine/utility/vulkan/raii_wrappers.hpp"
#include "engine/utility/vulkan/results.hpp"

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
    using SwapchainRecreatedSigh = entt::sigh<void(const vulkan::Swapchain&)>;
    using SwapchainRecreatedSink = entt::sink<SwapchainRecreatedSigh>;

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

    /*
VK_ERROR_OUT_OF_HOST_MEMORY
VK_ERROR_OUT_OF_DEVICE_MEMORY
VK_ERROR_DEVICE_LOST
VK_ERROR_OUT_OF_DATE_KHR
VK_ERROR_SURFACE_LOST_KHR
VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT
    */

    [[nodiscard]] auto acquire_next_image(
        vk::Semaphore t_semaphore = nullptr,
        vk::Fence     t_fence     = nullptr
    ) noexcept
        -> std::expected<
            std::variant<vulkan::err::Success, vulkan::err::SuboptimalKHR>,
            std::variant<
                err::NoSwapchain,
                vulkan::err::ErrorOutOfHostMemory,
                vulkan::err::ErrorOutOfDeviceMemory,
                vulkan::err::ErrorDeviceLost,
                vulkan::err::ErrorSurfaceLostKHR,
                vulkan::err::ErrorFullScreenExclusiveModeLostEXT>>;

    auto present(std::span<vk::Semaphore> t_wait_semaphores = {}) noexcept
        -> void;

    [[nodiscard]] auto swapchain_recreated_sink() noexcept
        -> SwapchainRecreatedSink&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vulkan::Surface                 m_surface;
    Device&                         m_device;
    std::function<vk::Extent2D()>   m_get_framebuffer_size;
    tl::optional<vulkan::Swapchain> m_swapchain;
    uint32_t                        m_image_index{};
    SwapchainRecreatedSigh          m_swapchain_recreated_signal;
    SwapchainRecreatedSink          m_swapchain_recreated_sink{
        m_swapchain_recreated_signal
    };

    ///***********///
    ///  Methods  ///
    ///***********///
    auto recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept -> void;
    auto recreate_swap_chain() noexcept -> void;
};

}   // namespace engine::renderer
