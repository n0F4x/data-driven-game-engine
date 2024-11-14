#pragma once

#include <VkBootstrap.h>

#include "core/image/Image.hpp"
#include "core/renderer/base/resources/Allocation.hpp"
#include "core/renderer/base/resources/Image.hpp"
#include "core/renderer/resources/SeqWriteBuffer.hpp"

#include "Image.hpp"

namespace core::gfx::resources {

class VirtualImage {
public:
    // NOLINTNEXTLINE(*-member-init)
    struct Block {
        std::vector<std::byte>                    m_source;
        std::optional<renderer::base::Allocation> m_allocation;
        bool                                      m_bound{};
        bool                                      m_uploaded{};
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
            const image::Image&              source
        );

        [[nodiscard]]
        auto operator()(
            vk::PhysicalDevice                  physical_device,
            vk::Queue                           sparse_queue,
            vk::CommandBuffer                   transfer_command_buffer,
            const renderer::base::Image::State& new_state
        ) && -> VirtualImage;

        [[nodiscard]]
        auto view() const -> vk::ImageView;

    private:
        renderer::base::Image m_image;
        vk::UniqueImageView   m_view;

        vk::MemoryRequirements            m_memory_requirements;
        vk::SparseImageMemoryRequirements m_sparse_requirements;

        std::vector<Block> m_blocks;

        MipTailRegion m_mip_tail_region;

        renderer::resources::SeqWriteBuffer<> m_mip_tail_staging_buffer;
        std::vector<vk::BufferImageCopy>      m_mip_tail_copy_regions;

        std::reference_wrapper<const renderer::base::Allocator> m_allocator;

        Image::Loader m_debug_image_loader;

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

    [[nodiscard]]
    auto image() const -> const renderer::base::Image&;

    [[nodiscard]]
    auto view() const -> vk::ImageView;

    [[nodiscard]]
    auto debug_image() const -> const Image&;

    [[nodiscard]]
    auto sparse_properties() const -> const vk::SparseImageMemoryRequirements&;

    [[nodiscard]]
    auto blocks() const -> std::span<const Block>;

    auto request_block(uint32_t block_index) -> void;

private:
    renderer::base::Image m_image;
    vk::UniqueImageView   m_view;

    vk::MemoryRequirements            m_memory_requirements;
    vk::SparseImageMemoryRequirements m_sparse_requirements;

    std::vector<Block> m_blocks;

    MipTailRegion m_mip_tail_region;

    std::vector<bool> m_to_be_loaded_mask;
    std::vector<bool> m_to_be_unloaded_mask;

    std::optional<core::renderer::resources::SeqWriteBuffer<>> m_staging_buffer;

    Image m_debug_image;

    VirtualImage(
        renderer::base::Image&&                  image,
        vk::UniqueImageView&&                    view,
        const vk::MemoryRequirements&            memory_requirements,
        const vk::SparseImageMemoryRequirements& sparse_requirements,
        std::vector<Block>&&                     blocks,
        MipTailRegion&&                          mip_tail_region,
        Image&&                                  debug_image
    );

    auto bind_memory_blocks(vk::Queue sparse_queue) -> void;

    auto upload_new_memory_blocks(
        const core::renderer::base::Allocator& allocator,
        vk::CommandBuffer                      transfer_command_buffer
    ) -> void;
};

}   // namespace core::gfx::resources
