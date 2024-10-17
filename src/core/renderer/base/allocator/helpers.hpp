#pragma once

#include <tuple>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer::base::details {

[[nodiscard]]
auto create_buffer(
    VmaAllocator                   allocator,
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) -> std::tuple<VmaAllocation, vk::Buffer, VmaAllocationInfo>;

auto copy(
    void*         dest,
    const void*   src,
    size_t        size,
    VmaAllocator  allocator,
    VmaAllocation allocation
) -> void;

template <VmaAllocationCreateFlags Flags>
auto create_mapped_buffer(
    VmaAllocator                allocator,
    const vk::BufferCreateInfo& buffer_create_info,
    const void*                 data = nullptr
) -> std::tuple<VmaAllocation, vk::Buffer, VmaAllocationInfo>;

}   // namespace core::renderer::details

#include "helpers.inl"
