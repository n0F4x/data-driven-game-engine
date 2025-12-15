module;

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#include "utility/lifetime_bound.hpp"

export module ddge.modules.vulkan.DeviceBuilder;

import vulkan_hpp;

import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.structure_chains.extends_struct_c;
import ddge.modules.vulkan.structure_chains.promoted_feature_struct_c;
import ddge.modules.vulkan.PhysicalDeviceSelector;

namespace ddge::vulkan {

export class DeviceBuilder {
public:
    explicit DeviceBuilder(PhysicalDeviceSelector&& physical_device_selector = {});

    template <typename Self_T>
    auto require_extension(this Self_T&&, const char* extension_name) -> Self_T;

    template <typename Self_T>
    auto require_features(this Self_T&&, const vk::PhysicalDeviceFeatures& features)
        -> Self_T;
    template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
        requires(!promoted_feature_struct_c<FeaturesStruct_T>)
    auto require_features(this Self_T&&, const FeaturesStruct_T& feature_struct)
        -> Self_T;

    template <typename Self_T>
    auto require_queue_flag(this Self_T&&, vk::QueueFlagBits flag) -> Self_T;
    template <typename Self_T>
    auto require_present_queue_for(
        this Self_T&&,
        [[lifetime_bound]] const vk::raii::SurfaceKHR& surface_khr
    ) -> Self_T;

    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T;

    [[nodiscard]]
    auto build(const vk::raii::Instance& instance) const
        -> std::optional<std::pair<vk::raii::PhysicalDevice, vk::raii::Device>>;

private:
    PhysicalDeviceSelector m_physical_device_selector;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

DeviceBuilder::DeviceBuilder(PhysicalDeviceSelector&& physical_device_selector)
    : m_physical_device_selector{ std::move(physical_device_selector) }
{}

template <typename Self_T>
auto DeviceBuilder::require_extension(this Self_T&& self, const char* const extension_name)
    -> Self_T
{
    self.m_physical_device_selector.require_extension(extension_name);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::require_features(
    this Self_T&&                     self,
    const vk::PhysicalDeviceFeatures& features
) -> Self_T
{
    self.m_physical_device_selector.require_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
    requires(!promoted_feature_struct_c<FeaturesStruct_T>)
auto DeviceBuilder::require_features(
    this Self_T&&           self,
    const FeaturesStruct_T& feature_struct
) -> Self_T
{
    self.m_physical_device_selector.require_features(feature_struct);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::require_queue_flag(this Self_T&& self, const vk::QueueFlagBits flag)
    -> Self_T
{
    self.m_physical_device_selector.require_queue_flag(flag);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::require_present_queue_for(
    this Self_T&&               self,
    const vk::raii::SurfaceKHR& surface_khr
) -> Self_T
{
    self.m_physical_device_selector.require_present_queue_for(surface_khr);
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto DeviceBuilder::add_custom_requirement(this Self_T&& self, Args_T&&... args) -> Self_T
{
    self.m_physical_device_selector.add_custom_requirement(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

auto DeviceBuilder::build(const vk::raii::Instance& instance) const
    -> std::optional<std::pair<vk::raii::PhysicalDevice, vk::raii::Device>>
{
    std::vector<vk::raii::PhysicalDevice> supported_devices{
        m_physical_device_selector.select_devices(instance)
    };

    if (supported_devices.empty()) {
        return std::nullopt;
    }

    vk::raii::PhysicalDevice physical_device{ std::move(supported_devices.front()) };

    const vk::DeviceCreateInfo device_create_info{
        .pNext = &m_physical_device_selector.required_features().root_struct(),
        .enabledExtensionCount =
            static_cast<uint32_t>(m_physical_device_selector.required_extensions().size()),
        .ppEnabledExtensionNames = m_physical_device_selector.required_extensions().data(),
    };

    vk::raii::Device device{
        check_result(physical_device.createDevice(device_create_info))
    };

    return std::make_pair(std::move(physical_device), std::move(device));
}

}   // namespace ddge::vulkan
