#include "Allocator.hpp"

#include <array>

#include <spdlog/spdlog.h>

#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"

using namespace core::renderer;

constexpr static VmaVulkanFunctions s_vulkan_functions{
    .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
    .vkGetDeviceProcAddr   = &vkGetDeviceProcAddr
};

[[nodiscard]]
static auto vma_allocator_create_flags(const vkb::PhysicalDevice& t_physical_device_info
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (t_physical_device_info.is_extension_present(
            VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME
        )
        && t_physical_device_info.is_extension_present(
            VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (t_physical_device_info.is_extension_present(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    }

    if (t_physical_device_info.is_extension_present(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    //    TODO: enable enabled device features as well
    flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    //    auto features{ vk::PhysicalDevice{ t_physical_device_info.physical_device }
    //                       .getFeatures2<
    //                           vk::PhysicalDeviceFeatures2,
    //                           vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
    //                           vk::PhysicalDeviceBufferDeviceAddressFeatures,
    //                           vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
    //                           vk::PhysicalDeviceMaintenance4Features>() };
    //
    //    if (t_physical_device_info.is_extension_present(
    //            VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME
    //        )
    //        &&
    //        features.get<vk::PhysicalDeviceCoherentMemoryFeaturesAMD>().deviceCoherentMemory)
    //    {
    //        flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    //    }
    //
    //    if (t_physical_device_info.is_extension_present(
    //            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
    //        )
    //        && features.get<vk::PhysicalDeviceBufferDeviceAddressFeatures>()
    //               .bufferDeviceAddress)
    //    {
    //        flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    //    }
    //
    //    if
    //    (t_physical_device_info.is_extension_present(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)
    //        &&
    //        features.get<vk::PhysicalDeviceMemoryPriorityFeaturesEXT>().memoryPriority)
    //    {
    //        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    //    }
    //
    //    if
    //    (t_physical_device_info.is_extension_present(VK_KHR_MAINTENANCE_4_EXTENSION_NAME)
    //        && features.get<vk::PhysicalDeviceMaintenance4Features>().maintenance4)
    //    {
    //        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    //    }
    //
    //    // TODO: VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT

    return flags;
}

[[nodiscard]]
static auto
    create_allocator(const Instance& t_instance, const Device& t_device) -> VmaAllocator
{
    const VmaAllocatorCreateInfo create_info{
        .flags            = vma_allocator_create_flags(t_device.info().physical_device),
        .physicalDevice   = t_device.physical_device(),
        .device           = t_device.get(),
        .pVulkanFunctions = &s_vulkan_functions,
        .instance         = t_instance.get(),
    };

    VmaAllocator     allocator;
    const vk::Result result{ vmaCreateAllocator(&create_info, &allocator) };
    vk::resultCheck(result, "vmaCreateAllocator");
    return allocator;
}

[[nodiscard]]
static auto create_buffer(
    const VmaAllocator             t_allocator,
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const VmaAllocationCreateInfo& t_allocation_create_info
) -> std::tuple<vk::Buffer, VmaAllocation, VmaAllocationInfo>
{
    vk::Buffer        buffer;
    VmaAllocation     allocation;
    VmaAllocationInfo allocation_info;

    const vk::Result result{ vmaCreateBuffer(
        t_allocator,
        reinterpret_cast<const VkBufferCreateInfo*>(&t_buffer_create_info),
        &t_allocation_create_info,
        reinterpret_cast<VkBuffer*>(&buffer),
        &allocation,
        &allocation_info
    ) };
    vk::resultCheck(result, "vmaCreateBuffer failed");

    return std::make_tuple(buffer, allocation, allocation_info);
}

[[nodiscard]]
static auto create_buffer_with_alignment(
    const VmaAllocator             t_allocator,
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const vk::DeviceSize           t_min_alignment,
    const VmaAllocationCreateInfo& t_allocation_create_info
) -> std::tuple<vk::Buffer, VmaAllocation, VmaAllocationInfo>
{
    vk::Buffer        buffer;
    VmaAllocation     allocation;
    VmaAllocationInfo allocation_info;

    const vk::Result result{ vmaCreateBufferWithAlignment(
        t_allocator,
        reinterpret_cast<const VkBufferCreateInfo*>(&t_buffer_create_info),
        &t_allocation_create_info,
        t_min_alignment,
        reinterpret_cast<VkBuffer*>(&buffer),
        &allocation,
        &allocation_info
    ) };
    vk::resultCheck(result, "vmaCreateBuffer failed");

    return std::make_tuple(buffer, allocation, allocation_info);
}

namespace core::renderer {

Allocator::Allocator(const Instance& t_instance, const Device& t_device)
    : m_allocator{ create_allocator(t_instance, t_device) }
{}

auto Allocator::operator*() const noexcept -> VmaAllocator
{
    return *m_allocator;
}

auto Allocator::operator->() const noexcept -> const VmaAllocator*
{
    return m_allocator.operator->();
}

auto Allocator::get() const noexcept -> VmaAllocator
{
    return m_allocator.get();
}

auto Allocator::create_buffer(
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const VmaAllocationCreateInfo& t_allocation_create_info
) const -> Buffer
{
    auto [buffer, allocation, _]{
        ::create_buffer(m_allocator.get(), t_buffer_create_info, t_allocation_create_info)
    };

    return Buffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::create_buffer_with_alignment(
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const vk::DeviceSize           t_min_alignment,
    const VmaAllocationCreateInfo& t_allocation_create_info
) const -> Buffer
{
    auto [buffer, allocation, _]{ ::create_buffer_with_alignment(
        m_allocator.get(), t_buffer_create_info, t_min_alignment, t_allocation_create_info
    ) };

    return Buffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::create_mapped_buffer(const vk::BufferCreateInfo& t_buffer_create_info
) const -> MappedBuffer
{
    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    auto [buffer, allocation, _]{
        ::create_buffer(m_allocator.get(), t_buffer_create_info, allocation_create_info)
    };

    return MappedBuffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::create_mapped_buffer_with_alignment(
    const vk::BufferCreateInfo& t_buffer_create_info,
    vk::DeviceSize              t_min_alignment
) const -> MappedBuffer
{
    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    auto [buffer, allocation, _]{ ::create_buffer_with_alignment(
        m_allocator.get(), t_buffer_create_info, t_min_alignment, allocation_create_info
    ) };

    return MappedBuffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::create_mapped_buffer(
    const vk::BufferCreateInfo& t_buffer_create_info,
    gsl::not_null<const void*>  t_data
) const -> MappedBuffer
{
    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    auto [buffer, allocation, allocation_info]{
        ::create_buffer(m_allocator.get(), t_buffer_create_info, allocation_create_info)
    };

    std::memcpy(allocation_info.pMappedData, t_data, t_buffer_create_info.size);

    vk::MemoryPropertyFlags memory_property_flags;
    vmaGetAllocationMemoryProperties(
        *m_allocator,
        allocation,
        reinterpret_cast<VkMemoryPropertyFlags*>(&memory_property_flags)
    );
    if (!(memory_property_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
        const vk::Result result{
            vmaFlushAllocation(*m_allocator, allocation, 0, VK_WHOLE_SIZE)
        };
        vk::resultCheck(result, "vmaFlushAllocation failed");
    }

    return MappedBuffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::create_mapped_buffer_with_alignment(
    const vk::BufferCreateInfo& t_buffer_create_info,
    vk::DeviceSize              t_min_alignment,
    gsl::not_null<const void*>  t_data
) const -> MappedBuffer
{
    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    auto [buffer, allocation, allocation_info]{ ::create_buffer_with_alignment(
        m_allocator.get(), t_buffer_create_info, t_min_alignment, allocation_create_info
    ) };

    std::memcpy(allocation_info.pMappedData, t_data, t_buffer_create_info.size);

    vk::MemoryPropertyFlags memory_property_flags;
    vmaGetAllocationMemoryProperties(
        *m_allocator,
        allocation,
        reinterpret_cast<VkMemoryPropertyFlags*>(&memory_property_flags)
    );
    if (!(memory_property_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
        const vk::Result result{
            vmaFlushAllocation(*m_allocator, allocation, 0, VK_WHOLE_SIZE)
        };
        vk::resultCheck(result, "vmaFlushAllocation failed");
    }

    return MappedBuffer{ buffer, allocation, m_allocator.get() };
}

}   // namespace core::renderer
