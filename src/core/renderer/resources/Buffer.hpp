#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <core/renderer/base/resources/Buffer.hpp>

#include "core/renderer/base/resources/Allocation.hpp"

namespace core::renderer::base {

class Allocator;

}   // namespace core::renderer::base

namespace core::renderer::resources {

class Buffer {
public:
    Buffer(
        const base::Allocator&         allocator,
        const vk::BufferCreateInfo&    buffer_create_info,
        const VmaAllocationCreateInfo& allocation_create_info =
            VmaAllocationCreateInfo{ .usage = VMA_MEMORY_USAGE_AUTO }
    );

    [[nodiscard]]
    auto buffer() const -> const base::Buffer&;
    [[nodiscard]]
    auto allocation() const -> const base::Allocation&;

    auto reset() noexcept -> void;

private:
    base::Buffer     m_buffer;
    base::Allocation m_allocation;

    [[nodiscard]]
    static auto make_from(std::tuple<base::Buffer, base::Allocation>&& tuple) noexcept
        -> Buffer;

    Buffer(base::Buffer&& buffer, base::Allocation&& allocation) noexcept;
};

}   // namespace core::renderer::resources
