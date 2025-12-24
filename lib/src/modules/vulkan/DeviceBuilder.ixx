module;

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <flat_map>
#include <functional>
#include <optional>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

export module ddge.modules.vulkan.DeviceBuilder;

import vulkan_hpp;

import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.structure_chains.extends_struct_c;
import ddge.modules.vulkan.structure_chains.merge_physical_device_features;
import ddge.modules.vulkan.structure_chains.promoted_feature_struct_c;
import ddge.modules.vulkan.structure_chains.remove_physical_device_features;
import ddge.modules.vulkan.structure_chains.StructureChain;
import ddge.modules.vulkan.PhysicalDeviceSelector;
import ddge.modules.vulkan.queue_properties;
import ddge.modules.vulkan.QueueGroup;
import ddge.modules.vulkan.QueuePack;
import ddge.utility.Bool;
import ddge.utility.containers.Lazy;
import ddge.utility.containers.StringLiteral;

namespace ddge::vulkan {

export class DeviceBuilder {
public:
    explicit DeviceBuilder(PhysicalDeviceSelector&& physical_device_selector = {});

    template <typename Self_T>
    auto enable_extension(this Self_T&&, util::StringLiteral extension_name) -> Self_T;
    template <typename Self_T>
    auto enable_extension_if_available(this Self_T&&, util::StringLiteral extension_name)
        -> Self_T;

    template <typename Self_T>
    auto enable_features(this Self_T&&, const vk::PhysicalDeviceFeatures& features)
        -> Self_T;
    template <typename Self_T>
    auto enable_features_if_available(
        this Self_T&&,
        const vk::PhysicalDeviceFeatures& features
    ) -> Self_T;
    template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
        requires(!promoted_feature_struct_c<FeaturesStruct_T>)
    auto enable_features(this Self_T&&, const FeaturesStruct_T& feature_struct) -> Self_T;
    template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
        requires(!promoted_feature_struct_c<FeaturesStruct_T>)
    auto enable_features_if_available(this Self_T&&, const FeaturesStruct_T& feature_struct)
        -> Self_T;

    template <typename Self_T>
    auto request_graphics_queue(this Self_T&&) -> Self_T;
    template <typename Self_T>
    auto request_compute_queue(this Self_T&&) -> Self_T;
    template <typename Self_T>
    auto request_host_to_device_transfer_queue(this Self_T&&) -> Self_T;
    template <typename Self_T>
    auto request_device_to_host_transfer_queue(this Self_T&&) -> Self_T;
    template <typename Self_T>
    auto request_dedicated_sparse_binding_queue(this Self_T&&) -> Self_T;
    template <typename Self_T>
    auto ensure_present_queue_for(this Self_T&&, vk::SurfaceKHR surface) -> Self_T;

    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T;

    template <typename Self_T>
    [[nodiscard]]
    auto build(this Self_T&& self, const vk::raii::Instance& instance) -> std::
        optional<std::tuple<vk::raii::PhysicalDevice, vk::raii::Device, QueueGroup>>;

private:
    [[nodiscard]]
    static auto make_queue_group(
        const vk::raii::Device&  device,
        QueueGroup::CreateInfo&& create_info
    ) -> QueueGroup;

    PhysicalDeviceSelector                      m_physical_device_selector;
    std::vector<util::StringLiteral>            m_optional_extension_names;
    StructureChain<vk::PhysicalDeviceFeatures2> m_optional_features;
    util::Bool                                  m_request_graphics_queue{};
    util::Bool                                  m_request_compute_queue{};
    bool                                        m_request_host_to_device_transfer_queue{};
    bool                                        m_request_device_to_host_transfer_queue{};
    std::vector<vk::SurfaceKHR>                 m_surfaces;
    bool m_request_dedicated_sparse_binding_queue{};

    [[nodiscard]]
    auto most_suitable(std::vector<vk::raii::PhysicalDevice>&& physical_devices) const
        -> std::optional<vk::raii::PhysicalDevice>;

    [[nodiscard]]
    auto create_device_queue_create_infos(
        const vk::raii::PhysicalDevice& physical_device
    ) const
        -> std::tuple<
            std::vector<std::vector<float>>,
            std::vector<vk::DeviceQueueCreateInfo>,
            QueueGroup::CreateInfo>;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

DeviceBuilder::DeviceBuilder(PhysicalDeviceSelector&& physical_device_selector)
    : m_physical_device_selector{ std::move(physical_device_selector) }
{}

template <typename Self_T>
auto DeviceBuilder::enable_extension(
    this Self_T&&             self,
    const util::StringLiteral extension_name
) -> Self_T
{
    if (const auto iter = std::ranges::find_if(
            self.m_optional_extension_names,
            [extension_name](const util::StringLiteral required_extension) -> bool {
                return std::strcmp(extension_name, required_extension) == 0;
            }
        );
        iter != self.m_optional_extension_names.cend())
    {
        self.m_optional_extension_names.erase(iter);
    }

    self.m_physical_device_selector.require_extension(extension_name);

    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::enable_extension_if_available(
    this Self_T&&       self,
    util::StringLiteral extension_name
) -> Self_T
{
    if (std::ranges::none_of(
            self.m_physical_device_selector.required_extensions(),
            [extension_name](const util::StringLiteral required_extension) -> bool {
                return std::strcmp(extension_name, required_extension) == 0;
            }
        ))
    {
        self.m_optional_extension_names.push_back(extension_name);
    }

    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::enable_features(
    this Self_T&&                     self,
    const vk::PhysicalDeviceFeatures& features
) -> Self_T
{
    remove_physical_device_features(
        self.m_optional_features.root_struct().features, features
    );
    self.m_physical_device_selector.require_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::enable_features_if_available(
    this Self_T&&                     self,
    const vk::PhysicalDeviceFeatures& features
) -> Self_T
{
    vk::PhysicalDeviceFeatures copy{ features };
    remove_physical_device_features(
        copy, self.m_physical_device_selector.required_features().root_struct().features
    );
    merge_physical_device_features(self.m_optional_features.root_struct().features, copy);
    return std::forward<Self_T>(self);
}

template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
    requires(!promoted_feature_struct_c<FeaturesStruct_T>)
auto DeviceBuilder::enable_features(
    this Self_T&&           self,
    const FeaturesStruct_T& feature_struct
) -> Self_T
{
    FeaturesStruct_T            copy{ feature_struct };
    vk::PhysicalDeviceFeatures2 features_chain{ .pNext = &copy };
    self.m_optional_features.remove_features(features_chain);
    self.m_physical_device_selector.require_features(feature_struct);
    return std::forward<Self_T>(self);
}

template <typename Self_T, extends_struct_c<vk::PhysicalDeviceFeatures2> FeaturesStruct_T>
    requires(!promoted_feature_struct_c<FeaturesStruct_T>)
auto DeviceBuilder::enable_features_if_available(
    this Self_T&&           self,
    const FeaturesStruct_T& feature_struct
) -> Self_T
{
    FeaturesStruct_T copy{ feature_struct };
    remove_physical_device_features(
        copy,
        self.m_physical_device_selector
            .required_features()[std::in_place_type<FeaturesStruct_T>]
    );
    vk::PhysicalDeviceFeatures2 root_struct{ .pNext = &copy };
    self.m_optional_features.merge(root_struct);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_graphics_queue(this Self_T&& self) -> Self_T
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eGraphics);
    self.m_request_graphics_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_compute_queue(this Self_T&& self) -> Self_T
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eCompute);
    self.m_request_compute_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_host_to_device_transfer_queue(this Self_T&& self) -> Self_T
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eTransfer);
    self.m_request_host_to_device_transfer_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_device_to_host_transfer_queue(this Self_T&& self) -> Self_T
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eTransfer);
    self.m_request_device_to_host_transfer_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_dedicated_sparse_binding_queue(this Self_T&& self) -> Self_T
{
    self.m_request_dedicated_sparse_binding_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::ensure_present_queue_for(
    this Self_T&&        self,
    const vk::SurfaceKHR surface
) -> Self_T
{
    self.m_surfaces.push_back(surface);

    self.add_custom_requirement(
        [surface](const vk::raii::PhysicalDevice& physical_device) -> bool {
            return first_present_queue_family_index(physical_device, surface).has_value();
        }
    );

    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto DeviceBuilder::add_custom_requirement(this Self_T&& self, Args_T&&... args) -> Self_T
{
    self.m_physical_device_selector.add_custom_requirement(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::build(this Self_T&& self, const vk::raii::Instance& instance)
    -> std::optional<std::tuple<vk::raii::PhysicalDevice, vk::raii::Device, QueueGroup>>
{
    std::vector<vk::raii::PhysicalDevice> supported_devices{
        self.m_physical_device_selector.select_devices(instance)
    };

    if (supported_devices.empty()) {
        return std::nullopt;
    }

    vk::raii::PhysicalDevice physical_device{
        *self.most_suitable(std::move(supported_devices))
    };

    std::vector<util::StringLiteral> extension_names{
        std::forward_like<Self_T>(self.m_physical_device_selector).required_extensions()
    };
    for (auto extension_properties{ physical_device.enumerateDeviceExtensionProperties() };
         util::StringLiteral optional_extension : self.m_optional_extension_names)
    {
        assert(
            std::ranges::none_of(
                extension_names,
                [optional_extension](const util::StringLiteral required_extension) -> bool {
                    return std::strcmp(optional_extension, required_extension) == 0;
                }
            )
        );
        if (std::ranges::any_of(
                extension_properties,
                [optional_extension](const char* const supported_extension) -> bool {
                    return std::strcmp(optional_extension, supported_extension) == 0;
                },
                &vk::ExtensionProperties::extensionName
            ))
        {
            extension_names.push_back(optional_extension);
        }
    }

    StructureChain<vk::PhysicalDeviceFeatures2> features{
        std::forward_like<Self_T>(self.m_physical_device_selector).required_features()
    };
    features.merge(self.m_optional_features.root_struct());
    StructureChain<vk::PhysicalDeviceFeatures2> supported_features{ features };
    physical_device.getDispatcher()->vkGetPhysicalDeviceFeatures2(
        *physical_device, supported_features.root_struct()
    );
    features.remove_unsupported_features(supported_features.root_struct());

    auto [per_family_queue_priorities, queue_create_infos, queue_group_create_info]{
        self.create_device_queue_create_infos(physical_device)
    };
    for (auto&& [priorities, queue_create_info] :
         std::views::zip(std::as_const(per_family_queue_priorities), queue_create_infos))
    {
        queue_create_info.pQueuePriorities = priorities.data();
    }

    const vk::DeviceCreateInfo device_create_info{
        .pNext                   = &features.root_struct(),
        .queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos       = queue_create_infos.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extension_names.size()),
        .ppEnabledExtensionNames = extension_names.front().address(),
    };

    vk::raii::Device device{
        check_result(physical_device.createDevice(device_create_info))
    };

    QueueGroup queue_group{ make_queue_group(device, std::move(queue_group_create_info)) };

    return std::make_tuple(
        std::move(physical_device), std::move(device), std::move(queue_group)
    );
}

auto DeviceBuilder::make_queue_group(
    const vk::raii::Device&  device,
    QueueGroup::CreateInfo&& create_info
) -> QueueGroup
{
    for (QueuePack& queue_pack : create_info.queue_packs) {
        const vk::DeviceQueueInfo2 queue_info{
            .queueFamilyIndex = queue_pack.family_index,
            .queueIndex       = queue_pack.queue_index,
        };
        queue_pack.queue = check_result(device.getQueue2(queue_info));
    }

    return QueueGroup(std::move(create_info));
}

auto DeviceBuilder::most_suitable(
    std::vector<vk::raii::PhysicalDevice>&& physical_devices
) const -> std::optional<vk::raii::PhysicalDevice>
{
    if (physical_devices.empty()) {
        return std::nullopt;
    }

    std::flat_multimap<uint32_t, vk::raii::PhysicalDevice, std::greater<uint32_t>>
        sorted_physical_devices;

    // TODO: use std::views::enumerate
    for (auto&& [index, physical_device] : std::views::zip(
             std::views::iota(0uz), std::views::as_rvalue(std::move(physical_devices))
         ))
    {
        uint32_t score{};

        if (m_request_dedicated_sparse_binding_queue
            && has_dedicated_sparse_binding_queue_family(physical_device))
        {
            score += 1 << 0;
        }
        if ((m_request_host_to_device_transfer_queue
             || m_request_device_to_host_transfer_queue)
            && has_dedicated_transfer_queue_family(physical_device))
        {
            score += 1 << 1;
        }
        if (m_request_compute_queue && has_dedicated_compute_queue_family(physical_device))
        {
            score += 1 << 2;
        }

        sorted_physical_devices.emplace(score, std::move(physical_device));
    }

    return std::move(sorted_physical_devices.begin()->second);
}

auto DeviceBuilder::create_device_queue_create_infos(
    const vk::raii::PhysicalDevice& physical_device
) const
    -> std::tuple<
        std::vector<std::vector<float>>,
        std::vector<vk::DeviceQueueCreateInfo>,
        QueueGroup::CreateInfo>
{
    std::tuple<
        std::vector<std::vector<float>>,
        std::vector<vk::DeviceQueueCreateInfo>,
        QueueGroup::CreateInfo>
                                     result{};
    std::vector<std::vector<float>>& per_family_queue_priorities{
        std::get<std::vector<std::vector<float>>>(result)
    };
    std::vector<vk::DeviceQueueCreateInfo>& device_queue_create_infos{
        std::get<std::vector<vk::DeviceQueueCreateInfo>>(result)
    };
    QueueGroup::CreateInfo& queue_group_create_info{
        std::get<QueueGroup::CreateInfo>(result)
    };

    const std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    const auto device_queue_create_info =                            //
        [&per_family_queue_priorities, &device_queue_create_infos]   //
        (
            const uint32_t family_index
        ) -> std::pair<vk::DeviceQueueCreateInfo&, std::vector<float>&>   //
    {
        const auto iter = std::ranges::find_if(
            device_queue_create_infos,   //
            [family_index](const vk::DeviceQueueCreateInfo& create_info) -> bool {
                return create_info.queueFamilyIndex == family_index;
            }
        );
        if (iter != device_queue_create_infos.cend()) {
            return std::pair<vk::DeviceQueueCreateInfo&, std::vector<float>&>{
                *iter,
                per_family_queue_priorities[static_cast<uint32_t>(
                    std::distance(device_queue_create_infos.begin(), iter)
                )]
            };
        }

        std::pair<vk::DeviceQueueCreateInfo&, std::vector<float>&> x_result{
            device_queue_create_infos.emplace_back(),
            per_family_queue_priorities.emplace_back()
        };
        x_result.first.queueFamilyIndex = family_index;
        return x_result;
    };

    const auto queue_count_can_be_incremented =
        [&device_queue_create_info, &queue_family_properties]   //
        [[nodiscard]]
        (const uint32_t family_index) -> bool                   //
    {
        return device_queue_create_info(family_index).first.queueCount
             < queue_family_properties[family_index].queueFamilyProperties.queueCount;
    };

    const auto try_setup_new_queue =   //
        [&device_queue_create_info,
         &queue_family_properties,
         &queue_group_create_info]   //
        (const uint32_t family_index, const float priority) -> std::optional<uint32_t>   //
    {
        const auto& [queue_create_info, queue_priorities]{
            device_queue_create_info(family_index)
        };

        if (queue_create_info.queueCount
            < queue_family_properties[family_index].queueFamilyProperties.queueCount)
        {
            queue_priorities.push_back(priority);
            ++queue_create_info.queueCount;

            queue_group_create_info.queue_packs.push_back(
                QueuePack{
                    .family_index = family_index,
                    .queue_index  = queue_create_info.queueCount - 1,
                    .queue        = nullptr,
                }
            );

            return queue_group_create_info.queue_packs.size() - 1;
        }

        return std::nullopt;
    };

    const auto next_incrementable_family_index =   //
        [&queue_family_properties, &queue_count_can_be_incremented](
            const vk::QueueFlagBits flag
        ) -> std::optional<uint32_t>   //
    {
        for (const uint32_t family_index :
             std::views::iota(0u, queue_family_properties.size()))
        {
            if (queue_family_properties[family_index].queueFamilyProperties.queueFlags
                    & flag
                && queue_count_can_be_incremented(family_index))
            {
                return family_index;
            }
        }
        return std::nullopt;
    };

    const std::optional<uint32_t> graphics_queue_family{
        m_request_graphics_queue.transform([&physical_device] -> uint32_t {
            return *first_graphics_queue_family_index(physical_device);
        })
    };

    if (m_request_graphics_queue) {
        queue_group_create_info.graphics_queue_pack_index =
            *try_setup_new_queue(*graphics_queue_family, 0.5f);
    }

    const std::optional<uint32_t> compute_queue_family{
        m_request_compute_queue.transform(
            [&physical_device,
             &next_incrementable_family_index,
             &graphics_queue_family] -> uint32_t   //
            {
                return first_dedicated_compute_queue_family_index(physical_device)
                    .or_else(
                        std::bind_front(
                            std::cref(next_incrementable_family_index),
                            vk::QueueFlagBits::eCompute
                        )
                    )
                    .value_or(
                        util::Lazy{
                            [&graphics_queue_family] -> uint32_t {
                                return *graphics_queue_family;
                            }   //
                        }
                    );
            }
        )   //
    };

    if (m_request_compute_queue) {
        queue_group_create_info.compute_queue_pack_index =
            try_setup_new_queue(*compute_queue_family, 0.5f)
                .value_or(   //
                    util::Lazy{
                        [&queue_group_create_info] -> uint32_t {
                            return *queue_group_create_info.graphics_queue_pack_index;
                        }   //
                    }
                );
    }

    const auto next_transfer_family = [&queue_count_can_be_incremented,
                                       &graphics_queue_family,
                                       &compute_queue_family,
                                       &next_incrementable_family_index](
                                          const vk::raii::PhysicalDevice& x_physical_device
                                      ) -> uint32_t   //
    {
        return first_dedicated_transfer_queue_family_index(x_physical_device)
            .or_else(
                std::bind_front(
                    std::cref(next_incrementable_family_index),
                    vk::QueueFlagBits::eTransfer
                )
            )
            .value_or(
                util::Lazy{
                    [&queue_count_can_be_incremented,
                     &graphics_queue_family,
                     &compute_queue_family] -> uint32_t   //
                    {
                        if (graphics_queue_family.has_value()
                            && queue_count_can_be_incremented(*graphics_queue_family))
                        {
                            return *graphics_queue_family;
                        }
                        if (compute_queue_family.has_value()
                            && queue_count_can_be_incremented(*compute_queue_family))
                        {
                            return *compute_queue_family;
                        }
                        std::unreachable();
                    }   //
                }
            );
    };

    const auto setup_new_transfer_queue =                        //
        [&queue_group_create_info, &try_setup_new_queue, this]   //
        (const uint32_t family_index) -> uint32_t                //
    {
        return try_setup_new_queue(family_index, 0.2f)
            .value_or(
                util::Lazy{
                    [&queue_group_create_info, this] -> uint32_t {
                        if (m_request_graphics_queue) {
                            return *queue_group_create_info.graphics_queue_pack_index;
                        }
                        if (m_request_compute_queue) {
                            return *queue_group_create_info.compute_queue_pack_index;
                        }
                        std::unreachable();
                    }   //
                }
            );
    };

    if (m_request_host_to_device_transfer_queue) {
        queue_group_create_info.host_to_device_transfer_queue_pack_index =
            setup_new_transfer_queue(next_transfer_family(physical_device));
    }

    if (m_request_device_to_host_transfer_queue) {
        queue_group_create_info.device_to_host_transfer_queue_pack_index =
            setup_new_transfer_queue(next_transfer_family(physical_device));
    }

    if (m_request_dedicated_sparse_binding_queue) {
        const std::optional<uint32_t> dedicated_sparse_binding_family{
            first_dedicated_transfer_queue_family_index(physical_device)
        };

        if (dedicated_sparse_binding_family.has_value()) {
            queue_group_create_info.dedicated_sparse_binding_queue_pack_index =
                try_setup_new_queue(*dedicated_sparse_binding_family, 0.7f);
        }
    }

    for (const vk::SurfaceKHR surface : m_surfaces) {
        if (std::ranges::none_of(
                queue_group_create_info.queue_packs,
                [surface, &physical_device](const uint32_t queue_family_index) -> bool {
                    return physical_device.getSurfaceSupportKHR(
                        queue_family_index, surface
                    );
                },
                &QueuePack::family_index
            ))
        {
            [[maybe_unused]]
            const bool success =
                try_setup_new_queue(
                    *first_present_queue_family_index(physical_device, surface), 0.5f
                )
                    .has_value();
            assert(success);
        }
    }

    return result;
}

}   // namespace ddge::vulkan
