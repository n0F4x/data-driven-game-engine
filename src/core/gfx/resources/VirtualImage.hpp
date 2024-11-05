#pragma once

#include <VkBootstrap.h>

#include "core/renderer/base/resources/Allocation.hpp"
#include "core/renderer/base/resources/Image.hpp"
#include "core/renderer/resources/SeqWriteBuffer.hpp"

namespace core::image {

class Image;

}   // namespace core::image

namespace core::gfx::resources {

class VirtualImage {
public:
    // NOLINTNEXTLINE(*-member-init)
    struct Block {
        std::optional<renderer::base::Allocation> m_allocation;
        vk::Offset3D                              m_offset;
        vk::Extent3D                              m_extent;
        vk::DeviceSize                            m_size;
        vk::ImageSubresource                      m_subresource;

        [[nodiscard]]
        auto buffer_size(vk::Format format) const -> uint64_t;
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
            vk::Queue                           sparse_queue,
            vk::CommandBuffer                   transfer_command_buffer,
            const renderer::base::Image::State& new_state
        ) && -> VirtualImage;

        [[nodiscard]]
        auto view() const -> vk::ImageView;

    private:
        std::unique_ptr<image::Image> m_source;
        renderer::base::Image         m_image;
        vk::UniqueImageView           m_view;

        vk::MemoryRequirements            m_memory_requirements;
        vk::SparseImageMemoryRequirements m_sparse_requirements;

        std::vector<Block> m_blocks;

        MipTailRegion m_mip_tail_region;

        renderer::resources::SeqWriteBuffer<> m_mip_tail_staging_buffer;

        std::reference_wrapper<const renderer::base::Allocator> m_allocator;

        auto bind_tail(vk::Queue sparse_queue) const -> void;
    };

    struct Requirements {
        static auto require_device_settings(
            vkb::PhysicalDeviceSelector& physical_device_selector
        ) -> void;
    };

    auto update(
        const renderer::base::Allocator& allocator,
        vk::Queue                        sparse_queue,
        vk::CommandBuffer                transfer_command_buffer
    ) -> void;
    auto clean_up_after_update() -> void;

private:
    std::unique_ptr<image::Image> m_source;
    renderer::base::Image         m_image;
    vk::UniqueImageView           m_view;

    vk::MemoryRequirements            m_memory_requirements;
    vk::SparseImageMemoryRequirements m_sparse_requirements;

    std::vector<Block> m_blocks;

    MipTailRegion m_mip_tail_region;

    std::vector<uint32_t> m_to_be_loaded_block_indices;
    std::vector<uint32_t> m_to_be_unloaded_block_indices;

    std::optional<core::renderer::resources::SeqWriteBuffer<>> m_staging_buffer;

    VirtualImage(
        std::unique_ptr<image::Image>&&          source,
        renderer::base::Image&&                  image,
        vk::UniqueImageView&&                    view,
        const vk::MemoryRequirements&            memory_requirements,
        const vk::SparseImageMemoryRequirements& sparse_requirements,
        std::vector<Block>&&                     blocks,
        MipTailRegion&&                          mip_tail_region
    );

    auto bind_memory_blocks(vk::Queue sparse_queue) -> void;

    auto upload_new_memory_blocks(
        const core::renderer::base::Allocator& allocator,
        vk::CommandBuffer                      transfer_command_buffer
    ) -> void;
};

}   // namespace core::gfx::resources
