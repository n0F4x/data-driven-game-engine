module;

#include <optional>
#include <vector>

#include <VkBootstrap.h>

export module modules.renderer.base.swapchain.Swapchain;

import vulkan_hpp;

namespace modules::renderer::base {

export class Swapchain {
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
    auto images() const noexcept -> std::span<const vk::Image>;
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
    std::vector<vk::Image>           m_images;
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

class Swapchain::Requirements {
public:
    [[nodiscard]]
    static auto required_instance_settings_are_available(
        const vkb::SystemInfo& system_info
    ) -> bool;

    static auto enable_instance_settings(
        const vkb::SystemInfo& system_info,
        vkb::InstanceBuilder&  builder
    ) -> void;

    static auto require_device_settings(
        vkb::PhysicalDeviceSelector& physical_device_selector
    ) -> void;

    static auto enable_optional_device_settings(vkb::PhysicalDevice& physical_device)
        -> void;
};

}   // namespace modules::renderer::base
