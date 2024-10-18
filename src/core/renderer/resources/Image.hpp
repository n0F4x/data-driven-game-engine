#pragma once

#include <vulkan/vulkan.hpp>

#include "core/image/Image.hpp"
#include "core/renderer/base/memory/Image.hpp"
#include "core/renderer/base/memory/SeqWriteBuffer.hpp"

namespace core::renderer::base {

class Allocator;

}   // namespace core::renderer::base

namespace core::renderer::resources {

class Image {
public:
    class Loader {
    public:
        Loader(
            vk::Device                device,
            const base::Allocator&    allocator,
            const core::image::Image& source
        );

        [[nodiscard]]
        auto operator()(
            vk::PhysicalDevice physical_device,
            vk::CommandBuffer  graphics_command_buffer
        ) && -> Image;

        [[nodiscard]]
        auto view() const -> vk::ImageView;

        [[nodiscard]]
        auto mip_level_count() const -> uint32_t;

        [[nodiscard]]
        auto needs_mip_generation() const -> bool;

    private:
        vk::Format                       m_format;
        vk::Extent3D                     m_extent;
        uint32_t                         m_mip_level_count;
        std::vector<vk::BufferImageCopy> m_copy_regions;

        base::Image         m_image;
        vk::UniqueImageView m_view;

        base::SeqWriteBuffer<> m_staging_buffer;
    };

private:
    base::Image         m_image;
    vk::UniqueImageView m_view;

    Image(base::Image&& image, vk::UniqueImageView&& view) noexcept;
};

}   // namespace core::renderer::resources
