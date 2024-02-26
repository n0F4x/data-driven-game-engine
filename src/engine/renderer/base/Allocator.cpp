#include "Allocator.hpp"

#include "Device.hpp"
#include "helpers.hpp"
#include "Instance.hpp"

static const VmaVulkanFunctions s_vulkan_functions{
    .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
    .vkGetDeviceProcAddr   = &vkGetDeviceProcAddr
};

[[nodiscard]] static auto
    create_allocator(const VmaAllocatorCreateInfo& t_vma_allocator_create_info)
        -> VmaAllocator
{
    VmaAllocator allocator;
    vk::Result   result{ vmaCreateAllocator(&t_vma_allocator_create_info, &allocator) };
    resultCheck(static_cast<vk::Result>(result), "vmaCreateAllocator");
    return allocator;
}

namespace engine::renderer {

Allocator::Allocator(const VmaAllocatorCreateInfo& t_vma_allocator_create_info)
    : m_allocator{ create_allocator(t_vma_allocator_create_info) }
{}

Allocator::Allocator(const Instance& t_instance, const Device& t_device)
    : Allocator{
          VmaAllocatorCreateInfo{
                                 .flags = helpers::vma_allocator_create_flags(
                                 t_instance.enabled_extensions(),
                                 t_device.info().extensions
                                 ), .physicalDevice   = t_device.physical_device(),
                                 .device           = *t_device,
                                 .pVulkanFunctions = &s_vulkan_functions,
                                 .instance         = *t_instance,
                                 }
}
{}

Allocator::Allocator(vma::Allocator&& t_allocator) noexcept
    : m_allocator{ std::move(t_allocator) }
{}

auto Allocator::operator*() const noexcept -> VmaAllocator
{
    return *m_allocator;
}

auto Allocator::operator->() const noexcept -> const VmaAllocator*
{
    return m_allocator.operator->();
}

auto Allocator::create_buffer(
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const VmaAllocationCreateInfo& t_allocation_create_info,
    const void*                    t_data
) const noexcept -> tl::optional<std::pair<vma::Buffer, VmaAllocationInfo>>
{
    vk::Buffer        buffer;
    VmaAllocation     allocation;
    VmaAllocationInfo allocation_info;
    if (const vk::Result result{ vmaCreateBuffer(
            *m_allocator,
            reinterpret_cast<const VkBufferCreateInfo*>(&t_buffer_create_info),
            &t_allocation_create_info,
            reinterpret_cast<VkBuffer*>(&buffer),
            &allocation,
            &allocation_info
        ) };
        result != vk::Result::eSuccess)
    {
        SPDLOG_WARN(
            "vmaCreateBuffer failed with error code {}", std::to_underlying(result)
        );
        return tl::nullopt;
    }

    vk::MemoryPropertyFlags memory_property_flags;
    vmaGetAllocationMemoryProperties(
        *m_allocator,
        allocation,
        reinterpret_cast<VkMemoryPropertyFlags*>(&memory_property_flags)
    );

    if (t_data != nullptr) {
        const bool mapped{ static_cast<bool>(
            t_allocation_create_info.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT
        ) };

        if (!mapped) {
            if (const vk::Result result{
                    vmaMapMemory(*m_allocator, allocation, &allocation_info.pMappedData) };
                result != vk::Result::eSuccess)
            {
                SPDLOG_WARN(
                    "vmaMapMemory failed with error code {}", std::to_underlying(result)
                );
                return tl::nullopt;
            }
        }

        memcpy(allocation_info.pMappedData, t_data, t_buffer_create_info.size);

        if (!(memory_property_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            if (const vk::Result result{
                    vmaFlushAllocation(*m_allocator, allocation, 0, VK_WHOLE_SIZE) };
                result != vk::Result::eSuccess)
            {
                SPDLOG_WARN(
                    "vmaFlushAllocation failed with error code {}",
                    std::to_underlying(result)
                );
                return tl::nullopt;
            }
        }

        if (!mapped) {
            vmaUnmapMemory(*m_allocator, allocation);
        }
    }

    return std::make_pair(
        vma::Buffer{ *m_allocator, buffer, allocation }, allocation_info
    );
}

}   // namespace engine::renderer
