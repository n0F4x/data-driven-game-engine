module;

#include <algorithm>
#include <cstdint>
#include <ranges>
#include <utility>
#include <vector>

export module ddge.modules.vulkan.PhysicalDeviceSelector;

import vulkan_hpp;

import ddge.modules.vulkan.PhysicalDeviceCapabilities;
import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.structure_chains.feature_struct_c;
import ddge.modules.vulkan.structure_chains.StructureChain;
import ddge.utility.containers.AnyCopyableFunction;
import ddge.utility.containers.StringLiteral;

namespace ddge::vulkan {

export class PhysicalDeviceSelector {
public:
    using CustomRequirement =
        util::AnyCopyableFunction<bool(const vk::raii::PhysicalDevice&) const>;

    template <typename Self_T>
    auto require_minimum_version(this Self_T&&, uint32_t version) -> Self_T;

    template <typename Self_T>
    auto require_extension(this Self_T&&, util::StringLiteral extension_name) -> Self_T;

    template <typename Self_T, feature_struct_c Features_T>
    auto require_features(this Self_T&&, const Features_T& features) -> Self_T;

    template <typename Self_T>
    auto require_queue_flag(this Self_T&&, vk::QueueFlagBits flag) -> Self_T;

    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T
        requires std::constructible_from<CustomRequirement, Args_T&&...>;

    [[nodiscard]]
    auto is_adequate(const vk::raii::PhysicalDevice& physical_device) const -> bool;

    [[nodiscard]]
    auto select_devices(const vk::raii::Instance& instance) const
        -> std::vector<vk::raii::PhysicalDevice>;

    [[nodiscard]]
    auto required_capabilities() const noexcept -> const PhysicalDeviceCapabilities&;
    [[nodiscard]]
    auto required_queue_flags() const -> vk::QueueFlags;

private:
    PhysicalDeviceCapabilities     m_required_capabilities;
    vk::QueueFlags                 m_queue_flags;
    std::vector<CustomRequirement> m_custom_requirements;

    [[nodiscard]]
    auto supports_queues_flags(const vk::raii::PhysicalDevice& physical_device) const
        -> bool;
    [[nodiscard]]
    auto supports_custom_requirements(
        const vk::raii::PhysicalDevice& physical_device
    ) const -> bool;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

template <typename Self_T>
auto PhysicalDeviceSelector::require_minimum_version(
    this Self_T&&  self,
    const uint32_t version
) -> Self_T
{
    self.m_required_capabilities.try_upgrade_version(version);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto PhysicalDeviceSelector::require_extension(
    this Self_T&&       self,
    util::StringLiteral extension_name
) -> Self_T
{
    self.m_required_capabilities.insert_extension(extension_name);
    return std::forward<Self_T>(self);
}

template <typename Self_T, feature_struct_c Features_T>
auto PhysicalDeviceSelector::require_features(
    this Self_T&&     self,
    const Features_T& features
) -> Self_T
{
    self.m_required_capabilities.insert_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto PhysicalDeviceSelector::require_queue_flag(
    this Self_T&&           self,
    const vk::QueueFlagBits flag
) -> Self_T
{
    self.m_queue_flags |= flag;
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto PhysicalDeviceSelector::add_custom_requirement(this Self_T&& self, Args_T&&... args)
    -> Self_T
    requires std::constructible_from<CustomRequirement, Args_T&&...>
{
    self.m_custom_requirements.emplace_back(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

auto PhysicalDeviceSelector::is_adequate(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    return m_required_capabilities.supported_by(physical_device)
        && supports_queues_flags(physical_device)   //
        && supports_custom_requirements(physical_device);
}

auto PhysicalDeviceSelector::select_devices(const vk::raii::Instance& instance) const
    -> std::vector<vk::raii::PhysicalDevice>
{
    std::vector<vk::raii::PhysicalDevice> result{
        check_result(instance.enumeratePhysicalDevices())
    };

    std::erase_if(result, [this](const vk::raii::PhysicalDevice& physical_device) -> bool {
        return !is_adequate(physical_device);
    });

    return result;
}

auto PhysicalDeviceSelector::required_capabilities() const noexcept
    -> const PhysicalDeviceCapabilities&
{
    return m_required_capabilities;
}

auto PhysicalDeviceSelector::required_queue_flags() const -> vk::QueueFlags
{
    return m_queue_flags;
}

auto PhysicalDeviceSelector::supports_queues_flags(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    const std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    vk::QueueFlags remaining_flags{ m_queue_flags };

    for (const vk::QueueFamilyProperties& queue_family :
         queue_family_properties
             | std::views::transform(&vk::QueueFamilyProperties2::queueFamilyProperties))
    {
        remaining_flags &= ~(remaining_flags & queue_family.queueFlags);

        if (!remaining_flags) {
            return true;
        }
    }

    return false;
}

auto PhysicalDeviceSelector::supports_custom_requirements(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    return std::ranges::all_of(
        m_custom_requirements,
        [&physical_device](const CustomRequirement& requirement) -> bool {
            return requirement(physical_device);
        }
    );
}

}   // namespace ddge::vulkan
