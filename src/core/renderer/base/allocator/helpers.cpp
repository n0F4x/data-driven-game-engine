#include "helpers.hpp"

#include <cstring>

namespace core::renderer::base::details {

auto create_buffer(
    const VmaAllocator             allocator,
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) -> std::tuple<VmaAllocation, vk::Buffer, VmaAllocationInfo>
{
    VmaAllocation     allocation{};
    vk::Buffer        buffer;
    VmaAllocationInfo allocation_info{};

    const vk::Result result{ vmaCreateBuffer(
        allocator,
        reinterpret_cast<const VkBufferCreateInfo*>(&buffer_create_info),
        &allocation_create_info,
        reinterpret_cast<VkBuffer*>(&buffer),
        &allocation,
        &allocation_info
    ) };
    vk::resultCheck(result, "vmaCreateBuffer failed");

    return std::make_tuple(allocation, buffer, allocation_info);
}

auto copy(
    void*               dest,
    const void*         src,
    const size_t        size,
    const VmaAllocator  allocator,
    const VmaAllocation allocation
) -> void
{
    std::memcpy(dest, src, size);

    vk::MemoryPropertyFlags memory_property_flags;
    vmaGetAllocationMemoryProperties(
        allocator,
        allocation,
        reinterpret_cast<VkMemoryPropertyFlags*>(&memory_property_flags)
    );
    if (!(memory_property_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
        const vk::Result result{ vmaFlushAllocation(allocator, allocation, 0, size) };
        vk::resultCheck(result, "vmaFlushAllocation failed");
    }
}

}   // namespace core::renderer::details
