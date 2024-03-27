#include "Allocator.hpp"

#include <array>

#include <spdlog/spdlog.h>

#include <vulkan/vulkan_extension_inspection.hpp>

#include "Device.hpp"
#include "Instance.hpp"

constexpr static VmaVulkanFunctions s_vulkan_functions{
    .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
    .vkGetDeviceProcAddr   = &vkGetDeviceProcAddr
};

[[nodiscard]] static auto enabled(
    std::span<const std::string> enabled_extension_names,
    const std::string&           t_extension_name
) -> bool
{
    return std::ranges::find(enabled_extension_names, t_extension_name)
            != std::cend(enabled_extension_names)
        || vk::isPromotedExtension(t_extension_name);
}

[[nodiscard]] static auto vma_allocator_create_flags(
    const std::span<const std::string> enabled_device_extensions
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (enabled(enabled_device_extensions, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (enabled(enabled_device_extensions, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    }

    if (enabled(enabled_device_extensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
    }

    if (enabled(enabled_device_extensions, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    if (enabled(enabled_device_extensions, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    }

    if (enabled(enabled_device_extensions, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    }

    if (enabled(enabled_device_extensions, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
    {
        flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    }

    return flags;
}

namespace core::renderer {

[[nodiscard]] static auto
    create_allocator(const Instance& t_instance, const Device& t_device) -> VmaAllocator
{
    const VmaAllocatorCreateInfo create_info{
        .flags            = vma_allocator_create_flags(t_device.enabled_extensions()),
        .physicalDevice   = t_device.physical_device(),
        .device           = t_device.get(),
        .pVulkanFunctions = &s_vulkan_functions,
        .instance         = t_instance.get(),
    };

    VmaAllocator     allocator;
    const vk::Result result{ vmaCreateAllocator(&create_info, &allocator) };
    resultCheck(result, "vmaCreateAllocator");
    return allocator;
}

auto Allocator::recommended_instance_extensions() noexcept -> std::span<const std::string>
{
    static const std::array<std::string, 5> s_extension_names{
        // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    };

    return s_extension_names;
}

auto Allocator::recommended_device_extensions() noexcept -> std::span<const std::string>
{
    static const std::array<std::string, 10> s_extension_names{
        // VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT
        VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
        VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
        VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
        VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT
        VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,

        // VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
    };

    return s_extension_names;
}

auto Allocator::recommended_device_extension_structs(
    const std::span<const std::string> t_enabled_device_extensions
) noexcept
    -> vk::StructureChain<
        vk::DeviceCreateInfo,
        vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
        vk::PhysicalDeviceBufferDeviceAddressFeatures,
        vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
        vk::PhysicalDeviceMaintenance4Features>
{
    vk::StructureChain<
        vk::DeviceCreateInfo,
        vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
        vk::PhysicalDeviceBufferDeviceAddressFeatures,
        vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
        vk::PhysicalDeviceMaintenance4Features>
        result;

    if (enabled(t_enabled_device_extensions, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
    {
        result.get<vk::PhysicalDeviceCoherentMemoryFeaturesAMD>().deviceCoherentMemory =
            true;
    }
    else {
        result.unlink<vk::PhysicalDeviceCoherentMemoryFeaturesAMD>();
    }

    if (enabled(t_enabled_device_extensions, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
    {
        result.get<vk::PhysicalDeviceBufferDeviceAddressFeatures>().bufferDeviceAddress =
            true;
    }
    else {
        result.unlink<vk::PhysicalDeviceBufferDeviceAddressFeatures>();
    }

    if (enabled(t_enabled_device_extensions, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
        result.get<vk::PhysicalDeviceMemoryPriorityFeaturesEXT>().memoryPriority = true;
    }
    else {
        result.unlink<vk::PhysicalDeviceMemoryPriorityFeaturesEXT>();
    }

    if (enabled(t_enabled_device_extensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        result.get<vk::PhysicalDeviceMaintenance4Features>().maintenance4 = true;
    }
    else {
        result.unlink<vk::PhysicalDeviceMaintenance4Features>();
    }

    return result;
}

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

}   // namespace core::renderer
