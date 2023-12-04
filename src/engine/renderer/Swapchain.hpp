#pragma once

#include <expected>
#include <functional>
#include <variant>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <entt/signal/sigh.hpp>

#include "engine/utility/Result.hpp"
#include "engine/utility/vulkan/raii_wrappers.hpp"
#include "engine/utility/vulkan/result_types.hpp"

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
    using FramebufferSizeGetter  = std::function<vk::Extent2D()>;
    using SwapchainRecreatedSigh = entt::sigh<void(const vulkan::Swapchain&)>;
    using SwapchainRecreatedSink = entt::sink<SwapchainRecreatedSigh>;

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
    FramebufferSizeGetter           m_get_framebuffer_size;
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
