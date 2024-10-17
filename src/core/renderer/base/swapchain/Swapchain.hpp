#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace core::renderer::base {

class Swapchain {
public:
    class Requirements;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]]
    static auto choose_extent(
        const vk::Extent2D&               framebuffer_size,
        const vk::SurfaceCapabilitiesKHR& surface_capabilities
    ) noexcept -> vk::Extent2D;

    [[nodiscard]]
    static auto create(
        vk::SurfaceKHR     surface,
        vk::PhysicalDevice physical_device,
        uint32_t           graphics_queue_family_index,
        uint32_t           present_queue_family_index,
        vk::Device         device,
        vk::Extent2D       framebuffer_size,
        vk::SwapchainKHR   old_swapchain = {}
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
    auto image_views() const noexcept -> std::span<const vk::UniqueImageView>;

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
