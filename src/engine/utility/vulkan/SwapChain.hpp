#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class SwapChain {
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit SwapChain(
        vk::Device                 t_device,
        vk::Extent2D               t_extent,
        vk::SurfaceFormatKHR       t_surface_format,
        vk::SwapchainKHR           t_swap_chain,
        std::vector<vk::ImageView> t_image_views
    ) noexcept;

public:
    SwapChain(SwapChain&&) noexcept;
    ~SwapChain() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(SwapChain&&) noexcept -> SwapChain& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::SwapchainKHR;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto extent() const noexcept -> vk::Extent2D;
    [[nodiscard]] auto image_views() const noexcept
        -> const std::vector<vk::ImageView>&;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        vk::SurfaceKHR                  t_surface,
        vk::PhysicalDevice              t_physical_device,
        uint32_t                        t_graphics_queue_family,
        uint32_t                        t_present_queue_family,
        vk::Device                      t_device,
        vk::Extent2D                    t_frame_buffer_size,
        std::optional<vk::SwapchainKHR> t_old_swap_chain
    ) noexcept -> std::optional<SwapChain>;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Device                 m_device;
    vk::Extent2D               m_extent;
    vk::SurfaceFormatKHR       m_surface_format;
    vk::SwapchainKHR           m_swap_chain;
    std::vector<vk::ImageView> m_image_views;
};

}   // namespace engine::vulkan
