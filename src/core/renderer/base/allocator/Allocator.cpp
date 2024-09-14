#include "Allocator.hpp"

#include <spdlog/spdlog.h>

#include "core/config/vulkan.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"
#include "core/renderer/memory/Buffer.hpp"
#include "core/renderer/memory/Image.hpp"

#include "helpers.hpp"

using namespace core;
using namespace core::renderer;

[[nodiscard]]
static auto vma_allocator_create_flags(const vkb::PhysicalDevice& physical_device_info
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (physical_device_info.is_extension_present(
            VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (physical_device_info.is_extension_present(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    }

    if (physical_device_info.is_extension_present(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    constexpr static vk::PhysicalDeviceCoherentMemoryFeaturesAMD
        coherent_memory_features_amd{
            .deviceCoherentMemory = vk::True,
        };
    if (physical_device_info.is_extension_present(
            VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME
        )
        && physical_device_info.are_extension_features_present(coherent_memory_features_amd
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    }

    constexpr static vk::PhysicalDeviceBufferDeviceAddressFeatures
        buffer_device_address_features{
            .bufferDeviceAddress = vk::True,
        };
    if (physical_device_info.are_extension_features_present(buffer_device_address_features
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    constexpr static vk::PhysicalDeviceMemoryPriorityFeaturesEXT memory_priority_features{
        .memoryPriority = vk::True,
    };
    if (physical_device_info.is_extension_present(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)
        && physical_device_info.are_extension_features_present(memory_priority_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance4Features maintenance4_features{
        .maintenance4 = vk::True,
    };
    if (physical_device_info.is_extension_present(VK_KHR_MAINTENANCE_4_EXTENSION_NAME)
        && physical_device_info.are_extension_features_present(maintenance4_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance5FeaturesKHR maintenance5_features{
        .maintenance5 = vk::True,
    };
    if (physical_device_info.is_extension_present(VK_KHR_MAINTENANCE_5_EXTENSION_NAME)
        && physical_device_info.are_extension_features_present(maintenance5_features))
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
        .vkGetInstanceProcAddr = config::vulkan::dispatcher().vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr   = config::vulkan::dispatcher().vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties =
            config::vulkan::dispatcher().vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties =
            config::vulkan::dispatcher().vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = config::vulkan::dispatcher().vkAllocateMemory,
        .vkFreeMemory     = config::vulkan::dispatcher().vkFreeMemory,
        .vkMapMemory      = config::vulkan::dispatcher().vkMapMemory,
        .vkUnmapMemory    = config::vulkan::dispatcher().vkUnmapMemory,
        .vkFlushMappedMemoryRanges =
            config::vulkan::dispatcher().vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges =
            config::vulkan::dispatcher().vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = config::vulkan::dispatcher().vkBindBufferMemory,
        .vkBindImageMemory  = config::vulkan::dispatcher().vkBindImageMemory,
        .vkGetBufferMemoryRequirements =
            config::vulkan::dispatcher().vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements =
            config::vulkan::dispatcher().vkGetImageMemoryRequirements,
        .vkCreateBuffer  = config::vulkan::dispatcher().vkCreateBuffer,
        .vkDestroyBuffer = config::vulkan::dispatcher().vkDestroyBuffer,
        .vkCreateImage   = config::vulkan::dispatcher().vkCreateImage,
        .vkDestroyImage  = config::vulkan::dispatcher().vkDestroyImage,
        .vkCmdCopyBuffer = config::vulkan::dispatcher().vkCmdCopyBuffer,
        .vkGetBufferMemoryRequirements2KHR =
            config::vulkan::dispatcher().vkGetBufferMemoryRequirements2KHR,
        .vkGetImageMemoryRequirements2KHR =
            config::vulkan::dispatcher().vkGetImageMemoryRequirements2KHR,
        .vkBindBufferMemory2KHR = config::vulkan::dispatcher().vkBindBufferMemory2KHR,
        .vkBindImageMemory2KHR  = config::vulkan::dispatcher().vkBindImageMemory2KHR,
        .vkGetPhysicalDeviceMemoryProperties2KHR =
            config::vulkan::dispatcher().vkGetPhysicalDeviceMemoryProperties2KHR,
        .vkGetDeviceBufferMemoryRequirements =
            config::vulkan::dispatcher().vkGetDeviceBufferMemoryRequirements,
        .vkGetDeviceImageMemoryRequirements =
            config::vulkan::dispatcher().vkGetDeviceImageMemoryRequirements,
    };

    return s_vulkan_functions;
}

[[nodiscard]]
static auto create_allocator(
    const Instance& instance,
    const Device&   device
) -> gsl_lite::not_null_ic<std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>>
{
    const VmaAllocatorCreateInfo create_info{
        .flags            = vma_allocator_create_flags(device.info().physical_device),
        .physicalDevice   = device.physical_device(),
        .device           = device.get(),
        .pVulkanFunctions = &vulkan_functions(),
        .instance         = instance.get(),
    };

    VmaAllocator     allocator{};
    const vk::Result result{ vmaCreateAllocator(&create_info, &allocator) };
    vk::resultCheck(result, "vmaCreateAllocator");
    return std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>{
        allocator, vmaDestroyAllocator
    };
}

namespace core::renderer {

Allocator::Allocator(const Instance& instance, const Device& device)
    : m_allocator{ create_allocator(instance, device) }
{}

auto Allocator::allocate_buffer(
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) const -> Buffer
{
    auto [allocation, buffer, _]{ details::create_buffer(
        m_allocator.get(), buffer_create_info, allocation_create_info
    ) };

    return Buffer{ m_allocator.get(), allocation, buffer, buffer_create_info.size };
}

auto Allocator::allocate_image(
    const vk::ImageCreateInfo&     image_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) const -> Image
{
    vk::Image        image{};
    VmaAllocation    allocation{};
    const vk::Result result{ vmaCreateImage(
        m_allocator.get(),
        reinterpret_cast<const VkImageCreateInfo*>(&image_create_info),
        &allocation_create_info,
        reinterpret_cast<VkImage*>(&image),
        &allocation,
        nullptr
    ) };
    vk::resultCheck(result, "vmaCreateImage failed");

    return Image(image, allocation, m_allocator.get());
}

}   // namespace core::renderer
