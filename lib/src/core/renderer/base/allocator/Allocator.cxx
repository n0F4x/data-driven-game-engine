module;

#include <gsl/gsl-lite.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <VkBootstrap.h>

#include "core/log/log.hpp"

module core.renderer.base.allocator.Allocator;

import core.renderer.base.resources.Allocation;
import core.renderer.base.resources.Buffer;
import core.renderer.base.resources.Image;

import core.config.vulkan;

import core.renderer.base.instance.Instance;
import core.renderer.base.device.Device;

[[nodiscard]]
static auto vma_allocator_create_flags(
    const vkb::PhysicalDevice& physical_device_info
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (physical_device_info.is_extension_present(vk::KHRDedicatedAllocationExtensionName))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (physical_device_info.is_extension_present(vk::KHRBindMemory2ExtensionName)) {
        flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    }

    if (physical_device_info.is_extension_present(vk::EXTMemoryBudgetExtensionName)) {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    constexpr static vk::PhysicalDeviceCoherentMemoryFeaturesAMD
        coherent_memory_features_amd{
            .deviceCoherentMemory = vk::True,
        };
    if (physical_device_info.is_extension_present(vk::AMDDeviceCoherentMemoryExtensionName)
        && physical_device_info.are_extension_features_present(
            coherent_memory_features_amd
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    }

    constexpr static vk::PhysicalDeviceBufferDeviceAddressFeatures
        buffer_device_address_features{
            .bufferDeviceAddress = vk::True,
        };
    if (physical_device_info.are_extension_features_present(buffer_device_address_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    constexpr static vk::PhysicalDeviceMemoryPriorityFeaturesEXT memory_priority_features{
        .memoryPriority = vk::True,
    };
    if (physical_device_info.is_extension_present(vk::EXTMemoryPriorityExtensionName)
        && physical_device_info.are_extension_features_present(memory_priority_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance4Features maintenance4_features{
        .maintenance4 = vk::True,
    };
    if (physical_device_info.is_extension_present(vk::KHRMaintenance4ExtensionName)
        && physical_device_info.are_extension_features_present(maintenance4_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    constexpr static vk::PhysicalDeviceMaintenance5FeaturesKHR maintenance5_features{
        .maintenance5 = vk::True,
    };
    if (physical_device_info.is_extension_present(vk::KHRMaintenance5ExtensionName)
        && physical_device_info.are_extension_features_present(maintenance5_features))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    return flags;
}

[[nodiscard]]
static auto get_vulkan_functions() -> VmaVulkanFunctions
{
    return VmaVulkanFunctions{
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
            core::config::vulkan::dispatcher().vkGetBufferMemoryRequirements2,
        .vkGetImageMemoryRequirements2KHR =
            core::config::vulkan::dispatcher().vkGetImageMemoryRequirements2,
        .vkBindBufferMemory2KHR = core::config::vulkan::dispatcher().vkBindBufferMemory2,
        .vkBindImageMemory2KHR  = core::config::vulkan::dispatcher().vkBindImageMemory2,
        .vkGetPhysicalDeviceMemoryProperties2KHR =
            core::config::vulkan::dispatcher().vkGetPhysicalDeviceMemoryProperties2,
        .vkGetDeviceBufferMemoryRequirements =
            core::config::vulkan::dispatcher().vkGetDeviceBufferMemoryRequirements,
        .vkGetDeviceImageMemoryRequirements =
            core::config::vulkan::dispatcher().vkGetDeviceImageMemoryRequirements,
    };
}

[[nodiscard]]
static auto create_allocator(
    const core::renderer::base::Instance& instance,
    const core::renderer::base::Device&   device
) -> gsl_lite::
    not_null_ic<std::unique_ptr<VmaAllocator_T, decltype(&::vmaDestroyAllocator)>>
{
    const VmaVulkanFunctions     vulkan_functions{ ::get_vulkan_functions() };
    const VmaAllocatorCreateInfo create_info{
        .flags            = ::vma_allocator_create_flags(device.info().physical_device),
        .physicalDevice   = device.physical_device(),
        .device           = device.get(),
        .pVulkanFunctions = &vulkan_functions,
        .instance         = instance.get(),
    };

    VmaAllocator     allocator{};
    const vk::Result result{ ::vmaCreateAllocator(&create_info, &allocator) };
    vk::detail::resultCheck(result, "vmaCreateAllocator");

    return std::unique_ptr<VmaAllocator_T, decltype(&::vmaDestroyAllocator)>{
        allocator, ::vmaDestroyAllocator
    };
}

core::renderer::base::Allocator::Allocator(const Instance& instance, const Device& device)
    : m_allocator{ ::create_allocator(instance, device) }
{}

auto core::renderer::base::Allocator::allocate(
    const vk::MemoryRequirements&  requirements,
    const VmaAllocationCreateInfo& allocation_create_info
) const -> std::tuple<Allocation, VmaAllocationInfo>
{
    VmaAllocation     allocation{};
    VmaAllocationInfo allocation_info{};

    const vk::Result result{ ::vmaAllocateMemory(
        m_allocator.get(),
        reinterpret_cast<const VkMemoryRequirements*>(&requirements),
        &allocation_create_info,
        &allocation,
        &allocation_info
    ) };
    vk::detail::resultCheck(result, "vmaAllocateMemory failed");

    return std::make_tuple(
        Allocation{ m_allocator.get(),
                    allocation,
                    allocation_info.memoryType,
                    allocation_info.size },
        allocation_info
    );
}

auto core::renderer::base::Allocator::create_buffer(
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) const -> std::tuple<Buffer, Allocation, VmaAllocationInfo>
{
    vk::Buffer        buffer;
    VmaAllocation     allocation{};
    VmaAllocationInfo allocation_info{};
    vk::detail::resultCheck(
        static_cast<vk::Result>(::vmaCreateBuffer(
            m_allocator.get(),
            reinterpret_cast<const VkBufferCreateInfo*>(&buffer_create_info),
            &allocation_create_info,
            reinterpret_cast<VkBuffer*>(&buffer),
            &allocation,
            &allocation_info
        )),
        "vmaCreateBuffer failed"
    );

    return std::make_tuple(
        Buffer{
            vk::UniqueBuffer{ buffer, device() },
            buffer_create_info.size
    },
        Allocation{ m_allocator.get(),
                    allocation,
                    allocation_info.memoryType,
                    allocation_info.size },
        allocation_info
    );
}

auto core::renderer::base::Allocator::create_image(
    const vk::ImageCreateInfo&     image_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) const -> std::tuple<Image, Allocation, VmaAllocationInfo>
{
    vk::Image         image{};
    VmaAllocation     allocation{};
    VmaAllocationInfo allocation_info{};
    vk::detail::resultCheck(
        static_cast<vk::Result>(::vmaCreateImage(
            m_allocator.get(),
            reinterpret_cast<const VkImageCreateInfo*>(&image_create_info),
            &allocation_create_info,
            reinterpret_cast<VkImage*>(&image),
            &allocation,
            &allocation_info
        )),
        "vmaCreateImage failed"
    );

    return std::make_tuple(
        Image{
            vk::UniqueImage{ image, device() },
            image_create_info
    },
        Allocation{ m_allocator.get(),
                    allocation,
                    allocation_info.memoryType,
                    allocation_info.size },
        allocation_info
    );
}

auto core::renderer::base::Allocator::device() const -> vk::Device
{
    ::VmaAllocatorInfo allocator_info;
    ::vmaGetAllocatorInfo(m_allocator.get(), &allocator_info);

    return allocator_info.device;
}
