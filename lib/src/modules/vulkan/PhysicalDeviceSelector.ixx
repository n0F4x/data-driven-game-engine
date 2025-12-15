module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <utility>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "utility/lifetime_bound.hpp"

export module ddge.modules.vulkan.PhysicalDeviceSelector;

import vulkan_hpp;

import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.structure_chains.extends_struct_c;
import ddge.modules.vulkan.structure_chains.match_physical_device_features;
import ddge.modules.vulkan.structure_chains.merge_physical_device_features;
import ddge.modules.vulkan.structure_chains.promoted_feature_struct_c;
import ddge.modules.vulkan.structure_chains.StructureChain;
import ddge.utility.containers.AnyCopyableFunction;
import ddge.utility.meta.concepts.naked;

namespace ddge::vulkan {

export class PhysicalDeviceSelector {
public:
    using CustomRequirement =
        util::AnyCopyableFunction<bool(const vk::raii::PhysicalDevice&) const>;

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
        [[lifetime_bound]] const vk::raii::SurfaceKHR& surface
    ) -> Self_T;

    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T
        requires std::constructible_from<CustomRequirement, Args_T&&...>;

    [[nodiscard]]
    auto is_adequate(const vk::raii::PhysicalDevice& physical_device) const -> bool;

    [[nodiscard]]
    auto select_devices(const vk::raii::Instance& instance) const
        -> std::vector<vk::raii::PhysicalDevice>;

    [[nodiscard]]
    auto required_extensions() const -> std::span<const char* const>;
    [[nodiscard]]
    auto required_features() const -> const StructureChain<vk::PhysicalDeviceFeatures2>&;
    [[nodiscard]]
    auto required_queue_flags() const -> vk::QueueFlags;

private:
    std::vector<const char*>                    m_required_extension_names;
    StructureChain<vk::PhysicalDeviceFeatures2> m_features_chain;
    vk::QueueFlags                              m_queue_flags;
    std::vector<std::reference_wrapper<const vk::raii::SurfaceKHR>> m_surfaces;
    std::vector<CustomRequirement>                                  m_custom_requirements;

    [[nodiscard]]
    auto supports_extensions(const vk::raii::PhysicalDevice& physical_device) const
        -> bool;
    [[nodiscard]]
    auto supports_features(const vk::raii::PhysicalDevice& physical_device) const -> bool;
    [[nodiscard]]
    auto supports_queues_flags(const vk::raii::PhysicalDevice& physical_device) const
        -> bool;
    [[nodiscard]]
    auto supports_surfaces(const vk::raii::PhysicalDevice& physical_device) const -> bool;
    [[nodiscard]]
    auto supports_custom_requirements(
        const vk::raii::PhysicalDevice& physical_device
    ) const -> bool;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

template <typename Self_T>
auto PhysicalDeviceSelector::require_extension(
    this Self_T&& self,
    const char*   extension_name
) -> Self_T
{
    if (std::ranges::none_of(
            self.m_required_extension_names,
            [extension_name](const char* const enabled_extension) -> bool {
                return std::strcmp(extension_name, enabled_extension) == 0;
            }
        ))
    {
        self.m_required_extension_names.push_back(extension_name);
    }

    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto PhysicalDeviceSelector::require_features(
    this Self_T&&                     self,
    const vk::PhysicalDeviceFeatures& features
) -> Self_T
{
    merge_physical_device_features(self.m_features_chain.root_struct().features, features);
    return std::forward<Self_T>(self);
}

template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
    requires(!promoted_feature_struct_c<FeaturesStruct_T>)
auto PhysicalDeviceSelector::require_features(
    this Self_T&&           self,
    const FeaturesStruct_T& feature_struct
) -> Self_T
{
    merge_physical_device_features(
        self.m_features_chain[std::in_place_type<FeaturesStruct_T>], feature_struct
    );
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

template <typename Self_T>
auto PhysicalDeviceSelector::require_present_queue_for(
    this Self_T&&               self,
    const vk::raii::SurfaceKHR& surface
) -> Self_T
{
    self.m_surfaces.push_back(surface);
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
    return supports_extensions(physical_device)     //
        && supports_features(physical_device)       //
        && supports_queues_flags(physical_device)   //
        && supports_surfaces(physical_device)
        && supports_custom_requirements(physical_device);
}

auto PhysicalDeviceSelector::select_devices(const vk::raii::Instance& instance) const
    -> std::vector<vk::raii::PhysicalDevice>
{
    std::vector<vk::raii::PhysicalDevice> result{
        check_result(instance.enumeratePhysicalDevices())
    };

    const auto [first, last] = std::ranges::remove_if(
        result, [this](const vk::raii::PhysicalDevice& physical_device) -> bool {
            return !is_adequate(physical_device);
        }
    );
    result.erase(first, last);

    return result;
}

auto PhysicalDeviceSelector::required_extensions() const -> std::span<const char* const>
{
    return m_required_extension_names;
}

auto PhysicalDeviceSelector::required_features() const
    -> const StructureChain<vk::PhysicalDeviceFeatures2>&
{
    return m_features_chain;
}

auto PhysicalDeviceSelector::required_queue_flags() const -> vk::QueueFlags
{
    return m_queue_flags;
}

auto PhysicalDeviceSelector::supports_extensions(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    const std::vector<vk::ExtensionProperties> supported_extension_properties{
        physical_device.enumerateDeviceExtensionProperties()
    };

    return std::ranges::all_of(
        m_required_extension_names,
        [supported_extensions = supported_extension_properties
                              | std::views::transform(
                                    &vk::ExtensionProperties::extensionName
                              )](const char* const required_extension) -> bool {
            return std::ranges::any_of(
                supported_extensions,
                [required_extension](const char* const supported_extension) -> bool {
                    return std::strcmp(required_extension, supported_extension) == 0;
                }
            );
        }
    );
}

auto PhysicalDeviceSelector::supports_features(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    StructureChain<vk::PhysicalDeviceFeatures2> supported_features{ m_features_chain };
    physical_device.getDispatcher()->vkGetPhysicalDeviceFeatures2(
        *physical_device, supported_features.root_struct()
    );

    return m_features_chain.matches(supported_features.root_struct());
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

auto PhysicalDeviceSelector::supports_surfaces(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    return std::ranges::all_of(
        m_surfaces, [&physical_device](const vk::raii::SurfaceKHR& surface) -> bool {
            return std::ranges::any_of(
                std::views::iota(0uz, physical_device.getQueueFamilyProperties2().size()),
                [&physical_device,
                 &surface](const std::size_t queue_family_index) -> bool {
                    return physical_device.getSurfaceSupportKHR(
                        static_cast<uint32_t>(queue_family_index), surface
                    );
                }
            );
        }
    );
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
