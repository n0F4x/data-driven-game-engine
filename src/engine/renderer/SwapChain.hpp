#pragma once

#include <optional>

#include "Device.hpp"

namespace engine::renderer {

class SwapChain {
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit SwapChain(
        const vk::raii::SurfaceKHR&       t_surface,
        const Device&                     t_device,
        const vk::SurfaceCapabilitiesKHR& t_surface_capabilities,
        const vk::Extent2D&               t_extent
    );

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto extent() const noexcept -> const vk::Extent2D&;
    [[nodiscard]] auto swap_chain() const noexcept
        -> const vk::raii::SwapchainKHR&;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        const vk::raii::SurfaceKHR& t_surface,
        const vk::Extent2D&         t_frame_buffer_size,
        const Device&               t_device
    ) -> std::optional<SwapChain>;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Extent2D                     m_extent;
    vk::SurfaceFormatKHR             m_surface_format;
    vk::raii::SwapchainKHR           m_swap_chain;
    std::vector<vk::raii::ImageView> m_imageViews;
};

}   // namespace engine::renderer
