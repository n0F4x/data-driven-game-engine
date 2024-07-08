#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace core::renderer {

class Swapchain {
public:
    class Requirements;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]]
    static auto choose_extent(
        const vk::Extent2D&               t_framebuffer_size,
        const vk::SurfaceCapabilitiesKHR& t_surface_capabilities
    ) noexcept -> vk::Extent2D;

    [[nodiscard]]
    static auto create(
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device,
        uint32_t           t_graphics_queue_family_index,
        uint32_t           t_present_queue_family_index,
        vk::Device         t_device,
        vk::Extent2D       t_framebuffer_size,
        vk::SwapchainKHR   t_old_swapchain = nullptr
    ) -> std::optional<Swapchain>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> vk::SwapchainKHR;
    [[nodiscard]]
    auto extent() const noexcept -> vk::Extent2D;
    [[nodiscard]]
    auto format() const noexcept -> vk::Format;
    [[nodiscard]]
    auto image_views() const noexcept -> const std::vector<vk::UniqueImageView>&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Device m_device;

    vk::Extent2D                     m_extent;
    vk::Format                       m_format;
    vk::UniqueSwapchainKHR           m_swapchain;
    std::vector<vk::UniqueImageView> m_image_views;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Swapchain(
        vk::Device                         device,
        vk::Extent2D                       extent,
        vk::Format                         format,
        vk::UniqueSwapchainKHR&&           swapchain,
        std::vector<vk::UniqueImageView>&& image_views
    ) noexcept;
};

}   // namespace core::renderer
