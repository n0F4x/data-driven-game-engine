module;

#include <ranges>

#include <vulkan/utility/vk_format_utils.h>


module ddge.deprecated.renderer.base.resources.Image;

ddge::renderer::base::Image::Image(
    vk::UniqueImage&&          image,
    const vk::ImageCreateInfo& create_info
)
    : m_image{ std::move(image) },
      m_format{ create_info.format },
      m_extent{ create_info.extent },
      m_mip_level_count{ create_info.mipLevels },
      m_state{ .layout = create_info.initialLayout }
{}

auto ddge::renderer::base::Image::get() const noexcept -> vk::Image
{
    return m_image.get();
}

auto ddge::renderer::base::Image::device() const noexcept -> vk::Device
{
    return m_image.getOwner();
}

auto ddge::renderer::base::Image::format() const noexcept -> vk::Format
{
    return m_format;
}

auto ddge::renderer::base::Image::aspect_flags() const noexcept -> vk::ImageAspectFlags
{
    vk::ImageAspectFlags result;

    if (vkuFormatIsColor(static_cast<VkFormat>(m_format))) {
        result |= vk::ImageAspectFlagBits::eColor;
    }
    if (vkuFormatHasDepth(static_cast<VkFormat>(m_format))) {
        result |= vk::ImageAspectFlagBits::eDepth;
    }
    if (vkuFormatHasStencil(static_cast<VkFormat>(m_format))) {
        result |= vk::ImageAspectFlagBits::eStencil;
    }

    return result;
}

auto ddge::renderer::base::Image::extent() const noexcept -> const vk::Extent3D&
{
    return m_extent;
}

auto ddge::renderer::base::Image::mip_level_count() const noexcept -> uint32_t
{
    return m_mip_level_count;
}

auto ddge::renderer::base::Image::layout() const noexcept -> vk::ImageLayout
{
    return m_state.layout;
}

auto ddge::renderer::base::Image::reset() -> void
{
    m_image.reset();
    m_format          = {};
    m_extent          = {};
    m_mip_level_count = {};
    m_state           = {};
}

auto ddge::renderer::base::Image::transition(const State& new_state) -> State
{
    return std::exchange(m_state, new_state);
}
