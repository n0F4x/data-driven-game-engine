#pragma once

#include <VkBootstrap.h>

#include "core/renderer/base/resources/Allocation.hpp"
#include "core/renderer/base/resources/Image.hpp"

#include "Image.hpp"

namespace core::renderer {
class Executor;
}   // namespace core::renderer

namespace core::image {

class Image;

}   // namespace core::image

namespace core::gfx::resources {

class VirtualImage {
public:
    // NOLINTNEXTLINE(*-member-init)
    struct Block {
        vk::Offset3D         m_offset;
        vk::Extent3D         m_extent;
        vk::DeviceSize       m_size;
        vk::ImageSubresource m_subresource;
    };

    struct MipRegion {
        std::vector<std::byte> raw_image_data;
    };

    struct MipTailRegion {
        renderer::base::Allocation m_memory;
    };

public:
    class Loader {
    public:
        Loader(
            vk::PhysicalDevice               physical_device,
            vk::Device                       device,
            const renderer::base::Allocator& allocator,
            std::unique_ptr<image::Image>&&  source
        );

        [[nodiscard]]
        auto operator()(
            vk::Queue         sparse_queue,
            vk::CommandBuffer graphics_command_buffer
        ) && -> VirtualImage;

        [[nodiscard]]
        auto view() const -> vk::ImageView;

    private:
        std::unique_ptr<image::Image> m_source;
        renderer::base::Image         m_image;
        vk::UniqueImageView           m_view;

        uint32_t                          m_element_size;
        vk::MemoryRequirements            m_memory_requirements;
        vk::SparseImageMemoryRequirements m_sparse_requirements;

        std::vector<Block> m_blocks;

        MipTailRegion m_mip_tail_region;

        auto bind_tail(vk::Queue sparse_queue) const -> void;
    };

    struct Requirements {
        static auto require_device_settings(
            vkb::PhysicalDeviceSelector& physical_device_selector
        ) -> void;
    };

private:
    std::unique_ptr<image::Image> m_source;
    renderer::base::Image         m_image;
    vk::UniqueImageView           m_view;

    VirtualImage(
        std::unique_ptr<image::Image>&& source,
        renderer::base::Image&&         image,
        vk::UniqueImageView&&           view
    ) noexcept;
};

}   // namespace core::gfx::resources
