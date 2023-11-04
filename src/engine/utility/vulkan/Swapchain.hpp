#pragma once

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

namespace engine::utils::vulkan {

class Swapchain {
public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto choose_extent(
        const vk::Extent2D&               t_framebuffer_size,
        const vk::SurfaceCapabilitiesKHR& t_surface_capabilities
    ) noexcept -> vk::Extent2D;

    [[nodiscard]] static auto create(
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device,
        uint32_t           t_graphics_queue_family,
        uint32_t           t_present_queue_family,
        vk::Device         t_device,
        vk::Extent2D       t_frame_buffer_size,
        vk::SwapchainKHR   t_old_swap_chain = nullptr
    ) noexcept -> tl::optional<Swapchain>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Swapchain(Swapchain&&) noexcept;
    ~Swapchain() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Swapchain&&) noexcept -> Swapchain& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::SwapchainKHR;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto extent() const noexcept -> vk::Extent2D;
    [[nodiscard]] auto surface_format() const noexcept -> vk::SurfaceFormatKHR;
    [[nodiscard]] auto image_views() const noexcept
        -> const std::vector<vk::ImageView>&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Device                 m_device;
    vk::Extent2D               m_extent;
    vk::SurfaceFormatKHR       m_surface_format;
    vk::SwapchainKHR           m_swap_chain;
    std::vector<vk::ImageView> m_image_views;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Swapchain(
        vk::Device                 t_device,
        vk::Extent2D               t_extent,
        vk::SurfaceFormatKHR       t_surface_format,
        vk::SwapchainKHR           t_swap_chain,
        std::vector<vk::ImageView> t_image_views
    ) noexcept;
};

}   // namespace engine::utils::vulkan
