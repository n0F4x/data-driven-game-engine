#include "Requirements.hpp"

namespace core::renderer {

[[nodiscard]] static auto
    optional_instance_extension_names() noexcept -> std::span<const std::string>
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

auto Allocator::Requirements::
    required_instance_settings_are_available(const vkb::SystemInfo&) -> bool
{
    return true;
}

auto Allocator::Requirements::enable_instance_settings(
    const vkb::SystemInfo& t_system_info,
    vkb::InstanceBuilder&  t_builder
) -> void
{
    for (const auto& extension_name : optional_instance_extension_names()) {
        if (t_system_info.is_extension_available(extension_name.c_str())) {
            t_builder.enable_extension(extension_name.c_str());
        }
    }
}

auto Allocator::Requirements::require_device_settings(vkb::PhysicalDeviceSelector&) -> void
{}

auto Allocator::Requirements::enable_optional_device_settings(
    vkb::PhysicalDevice& t_physical_device
) -> void
{
    // VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
    t_physical_device.enable_extension_if_present(
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME
    );
    t_physical_device.enable_extension_if_present(
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME
    );

    // VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT
    t_physical_device.enable_extension_if_present(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);

    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
    t_physical_device.enable_extension_if_present(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);

    // TODO: enable optional features and their extensions as well
    // TODO: See https://github.com/charles-lunarg/vk-bootstrap/issues/269
}

auto Allocator::Requirements::optional_device_extension_structs(
    const vkb::PhysicalDevice& t_physical_device
)
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

    if (t_physical_device.is_extension_present(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME
        ))
    {
        result.get<vk::PhysicalDeviceCoherentMemoryFeaturesAMD>().deviceCoherentMemory =
            true;
    }
    else {
        result.unlink<vk::PhysicalDeviceCoherentMemoryFeaturesAMD>();
    }

    if (t_physical_device.is_extension_present(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
        ))
    {
        result.get<vk::PhysicalDeviceBufferDeviceAddressFeatures>().bufferDeviceAddress =
            true;
    }
    else {
        result.unlink<vk::PhysicalDeviceBufferDeviceAddressFeatures>();
    }

    if (t_physical_device.is_extension_present(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
        result.get<vk::PhysicalDeviceMemoryPriorityFeaturesEXT>().memoryPriority = true;
    }
    else {
        result.unlink<vk::PhysicalDeviceMemoryPriorityFeaturesEXT>();
    }

    if (t_physical_device.is_extension_present(VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        result.get<vk::PhysicalDeviceMaintenance4Features>().maintenance4 = true;
    }
    else {
        result.unlink<vk::PhysicalDeviceMaintenance4Features>();
    }

    return result;
}

}   // namespace core::renderer
