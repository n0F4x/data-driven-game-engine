#include "Allocator.hpp"

#include <array>

#include <spdlog/spdlog.h>

#include "core/config/vulkan.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"

using namespace core::renderer;

[[nodiscard]]
static auto vma_allocator_create_flags(const vkb::PhysicalDevice& t_physical_device_info
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (t_physical_device_info.is_extension_present(
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

    constexpr static vk::PhysicalDeviceCoherentMemoryFeaturesAMD
        coherent_memory_features_amd{
            .deviceCoherentMemory = vk::True,
        };
    if (t_physical_device_info.is_extension_present(
            VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME
        )
        && t_physical_device_info.are_extension_features_present(
            coherent_memory_features_amd
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    }

    constexpr static vk::PhysicalDeviceBufferDeviceAddressFeatures
        buffer_device_address_features{
            .bufferDeviceAddress = vk::True,
        };
    if (t_physical_device_info.are_extension_features_present(
            buffer_device_address_features
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    constexpr static vk::PhysicalDeviceMemoryPriorityFeaturesEXT memory_priority_features{
        .memoryPriority = vk::True,
    };
    if (t_physical_device_info.is_extension_present(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)
        && t_physical_device_info.are_extension_features_present(memory_priority_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance4Features maintenance4_features{
        .maintenance4 = vk::True,
    };
    if (t_physical_device_info.is_extension_present(VK_KHR_MAINTENANCE_4_EXTENSION_NAME)
        && t_physical_device_info.are_extension_features_present(maintenance4_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance5FeaturesKHR maintenance5_features{
        .maintenance5 = vk::True,
    };
    if (t_physical_device_info.is_extension_present(VK_KHR_MAINTENANCE_5_EXTENSION_NAME)
        && t_physical_device_info.are_extension_features_present(maintenance5_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    return flags;
}

[[nodiscard]]
static auto vulkan_functions() -> const VmaVulkanFunctions&
{
    // TODO: use reflection
    static const VmaVulkanFunctions s_vulkan_functions{
        .vkGetInstanceProcAddr = core::config::vulkan::dispatcher().vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr   = core::config::vulkan::dispatcher().vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties =
            core::config::vulkan::dispatcher().vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties =
            core::config::vulkan::dispatcher().vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = core::config::vulkan::dispatcher().vkAllocateMemory,
        .vkFreeMemory     = core::config::vulkan::dispatcher().vkFreeMemory,
        .vkMapMemory      = core::config::vulkan::dispatcher().vkMapMemory,
        .vkUnmapMemory    = core::config::vulkan::dispatcher().vkUnmapMemory,
        .vkFlushMappedMemoryRanges =
            core::config::vulkan::dispatcher().vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges =
            core::config::vulkan::dispatcher().vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = core::config::vulkan::dispatcher().vkBindBufferMemory,
        .vkBindImageMemory  = core::config::vulkan::dispatcher().vkBindImageMemory,
        .vkGetBufferMemoryRequirements =
            core::config::vulkan::dispatcher().vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements =
            core::config::vulkan::dispatcher().vkGetImageMemoryRequirements,
        .vkCreateBuffer  = core::config::vulkan::dispatcher().vkCreateBuffer,
        .vkDestroyBuffer = core::config::vulkan::dispatcher().vkDestroyBuffer,
        .vkCreateImage   = core::config::vulkan::dispatcher().vkCreateImage,
        .vkDestroyImage  = core::config::vulkan::dispatcher().vkDestroyImage,
        .vkCmdCopyBuffer = core::config::vulkan::dispatcher().vkCmdCopyBuffer,
        .vkGetBufferMemoryRequirements2KHR =
            core::config::vulkan::dispatcher().vkGetBufferMemoryRequirements2KHR,
        .vkGetImageMemoryRequirements2KHR =
            core::config::vulkan::dispatcher().vkGetImageMemoryRequirements2KHR,
        .vkBindBufferMemory2KHR =
            core::config::vulkan::dispatcher().vkBindBufferMemory2KHR,
        .vkBindImageMemory2KHR = core::config::vulkan::dispatcher().vkBindImageMemory2KHR,
        .vkGetPhysicalDeviceMemoryProperties2KHR =
            core::config::vulkan::dispatcher().vkGetPhysicalDeviceMemoryProperties2KHR,
        .vkGetDeviceBufferMemoryRequirements =
            core::config::vulkan::dispatcher().vkGetDeviceBufferMemoryRequirements,
        .vkGetDeviceImageMemoryRequirements =
            core::config::vulkan::dispatcher().vkGetDeviceImageMemoryRequirements,
    };

    return s_vulkan_functions;
}

[[nodiscard]]
static auto create_allocator(
    const Instance& t_instance,
    const Device&   t_device
) -> gsl_lite::not_null_ic<std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>>
{
    const VmaAllocatorCreateInfo create_info{
        .flags            = vma_allocator_create_flags(t_device.info().physical_device),
        .physicalDevice   = t_device.physical_device(),
        .device           = t_device.get(),
        .pVulkanFunctions = &vulkan_functions(),
        .instance         = t_instance.get(),
    };

    VmaAllocator     allocator{};
    const vk::Result result{ vmaCreateAllocator(&create_info, &allocator) };
    vk::resultCheck(result, "vmaCreateAllocator");
    return std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>{
        allocator, vmaDestroyAllocator
    };
}

[[nodiscard]]
static auto create_buffer(
    const VmaAllocator             t_allocator,
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const VmaAllocationCreateInfo& t_allocation_create_info
) -> std::tuple<vk::Buffer, VmaAllocation, VmaAllocationInfo>
{
    vk::Buffer        buffer;
    VmaAllocation     allocation{};
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
    VmaAllocation     allocation{};
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

auto Allocator::operator*() const noexcept -> const VmaAllocator_T&
{
    return *m_allocator;
}

auto Allocator::operator*() noexcept -> VmaAllocator_T&
{
    return *m_allocator;
}

auto Allocator::operator->() const noexcept -> VmaAllocator
{
    return m_allocator.operator->().operator->();
}

auto Allocator::get() const noexcept -> VmaAllocator
{
    return m_allocator.get();
}

auto Allocator::allocate_buffer(
    const vk::BufferCreateInfo&    t_buffer_create_info,
    const VmaAllocationCreateInfo& t_allocation_create_info
) const -> Buffer
{
    auto [buffer, allocation, _]{
        ::create_buffer(m_allocator.get(), t_buffer_create_info, t_allocation_create_info)
    };

    return Buffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::allocate_buffer_with_alignment(
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

auto Allocator::allocate_mapped_buffer(const vk::BufferCreateInfo& t_buffer_create_info
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

auto Allocator::allocate_mapped_buffer_with_alignment(
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

auto Allocator::allocate_mapped_buffer(
    const vk::BufferCreateInfo&        t_buffer_create_info,
    gsl_lite::not_null_ic<const void*> t_data
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
        m_allocator.get(),
        allocation,
        reinterpret_cast<VkMemoryPropertyFlags*>(&memory_property_flags)
    );
    if (!(memory_property_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
        const vk::Result result{
            vmaFlushAllocation(m_allocator.get(), allocation, 0, vk::WholeSize)
        };
        vk::resultCheck(result, "vmaFlushAllocation failed");
    }

    return MappedBuffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::allocate_mapped_buffer_with_alignment(
    const vk::BufferCreateInfo&        t_buffer_create_info,
    vk::DeviceSize                     t_min_alignment,
    gsl_lite::not_null_ic<const void*> t_data
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
        m_allocator.get(),
        allocation,
        reinterpret_cast<VkMemoryPropertyFlags*>(&memory_property_flags)
    );
    if (!(memory_property_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
        const vk::Result result{
            vmaFlushAllocation(m_allocator.get(), allocation, 0, vk::WholeSize)
        };
        vk::resultCheck(result, "vmaFlushAllocation failed");
    }

    return MappedBuffer{ buffer, allocation, m_allocator.get() };
}

auto Allocator::allocate_image(
    const vk::ImageCreateInfo&     t_image_create_info,
    const VmaAllocationCreateInfo& t_allocation_create_info
) const -> Image
{
    vk::Image        image{};
    VmaAllocation    allocation{};
    const vk::Result result{ vmaCreateImage(
        m_allocator.get(),
        reinterpret_cast<const VkImageCreateInfo*>(&t_image_create_info),
        &t_allocation_create_info,
        reinterpret_cast<VkImage*>(&image),
        &allocation,
        nullptr
    ) };
    vk::resultCheck(result, "vmaCreateImage failed");

    return renderer::Image(image, allocation, m_allocator.get());
}

}   // namespace core::renderer
