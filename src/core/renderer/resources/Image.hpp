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
        auto operator()(vk::CommandBuffer transfer_command_buffer) && -> Image;

        [[nodiscard]]
        auto view() const -> vk::ImageView;

    private:
        base::Image         m_image;
        vk::UniqueImageView m_view;

        base::SeqWriteBuffer<> m_staging_buffer;

        vk::Extent3D   m_extent;
        vk::DeviceSize m_buffer_offset;
    };

private:
    base::Image         m_image;
    vk::UniqueImageView m_view;

    Image(base::Image&& image, vk::UniqueImageView&& view) noexcept;
};

}   // namespace core::renderer::resources
