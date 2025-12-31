module;

#include <algorithm>
#include <cstdint>
#include <ranges>
#include <vector>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.PhysicalDeviceCapabilities;

import vulkan_hpp;

import ddge.modules.vulkan.extension_inspection.try_promote_extension_to_vulkan1x;
import ddge.modules.vulkan.structure_chains.core_feature_struct_from_vulkan1x_c;
import ddge.modules.vulkan.structure_chains.individual_feature_struct_c;
import ddge.modules.vulkan.structure_chains.merge_physical_device_features;
import ddge.modules.vulkan.structure_chains.remove_physical_device_features;
import ddge.modules.vulkan.structure_chains.StructureChain;
import ddge.modules.vulkan.structure_chains.vulkan1x_feature_struct_c;
import ddge.utility.containers.OptionalRef;
import ddge.utility.containers.StringLiteral;
import ddge.utility.contracts;

namespace ddge::vulkan {

export class PhysicalDeviceCapabilities {
public:
    [[nodiscard]]
    auto api_version() const noexcept -> uint32_t;
    [[nodiscard]]
    auto extensions() const noexcept -> std::span<const util::StringLiteral>;
    [[nodiscard]]
    auto features() const noexcept -> const StructureChain<vk::PhysicalDeviceFeatures2>&;

    [[nodiscard]]
    auto supported_by(const vk::raii::PhysicalDevice& physical_device) const -> bool;

    auto filter_uncontained_features(vk::PhysicalDeviceFeatures& features) const -> void;
    template <vulkan1x_feature_struct_c Vulkan1XFeatureStruct_T>
    auto filter_uncontained_features(Vulkan1XFeatureStruct_T& features) const -> void;
    template <individual_feature_struct_c FeaturesStruct_T>
    auto filter_uncontained_features(FeaturesStruct_T& features) const -> void;


    auto try_upgrade_api_version(uint32_t new_api_version) -> void;

    auto insert_extension(util::StringLiteral extension_name) -> void;

    auto insert_features(const vk::PhysicalDeviceFeatures& features) -> void;
    auto insert_features(const vk::PhysicalDeviceVulkan11Features& features) -> void;
    auto insert_features(const vk::PhysicalDeviceVulkan12Features& features) -> void;
    auto insert_features(const vk::PhysicalDeviceVulkan13Features& features) -> void;
    auto insert_features(const vk::PhysicalDeviceVulkan14Features& features) -> void;
    template <individual_feature_struct_c FeaturesStruct_T>
    auto insert_features(const FeaturesStruct_T& features) -> void;
    auto insert_features(StructureChain<vk::PhysicalDeviceFeatures2> features) -> void;


    auto try_remove_extension(util::StringLiteral extension_name) -> void;

    auto try_remove_features(const vk::PhysicalDeviceFeatures& features) -> void;
    auto try_remove_features(const vk::PhysicalDeviceVulkan11Features& features) -> void;
    auto try_remove_features(const vk::PhysicalDeviceVulkan12Features& features) -> void;
    auto try_remove_features(const vk::PhysicalDeviceVulkan13Features& features) -> void;
    auto try_remove_features(const vk::PhysicalDeviceVulkan14Features& features) -> void;
    template <individual_feature_struct_c FeaturesStruct_T>
    auto try_remove_features(const FeaturesStruct_T& features) -> void;

private:
    uint32_t                                    m_api_version{ vk::ApiVersion11 };
    std::vector<util::StringLiteral>            m_extension_names{};
    StructureChain<vk::PhysicalDeviceFeatures2> m_features;

    auto upgrade_to_Vulkan11() -> void;
    auto upgrade_to_Vulkan12() -> void;
    auto upgrade_to_Vulkan13() -> void;
    auto upgrade_to_Vulkan14() -> void;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

auto PhysicalDeviceCapabilities::api_version() const noexcept -> uint32_t
{
    return m_api_version;
}

auto PhysicalDeviceCapabilities::extensions() const noexcept
    -> std::span<const util::StringLiteral>
{
    return m_extension_names;
}

auto PhysicalDeviceCapabilities::features() const noexcept
    -> const StructureChain<vk::PhysicalDeviceFeatures2>&
{
    return m_features;
}

auto PhysicalDeviceCapabilities::supported_by(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    const vk::PhysicalDeviceProperties properties{ physical_device.getProperties() };
    if (properties.apiVersion < m_api_version) {
        return false;
    }

    const std::vector<vk::ExtensionProperties> supported_extension_properties{
        physical_device.enumerateDeviceExtensionProperties()
    };
    if (std::ranges::any_of(
            m_extension_names,
            [supported_extensions = supported_extension_properties
                                  | std::ranges::views::transform(
                                        &vk::ExtensionProperties::extensionName
                                  )]                             //
            (const util::StringLiteral extension_name) -> bool   //
            {
                return std::ranges::none_of(
                    supported_extensions,
                    [extension_name](const char* const supported_extension) -> bool {
                        return extension_name
                            == util::StringLiteral::unsafe_create(supported_extension);
                    }
                );
            }
        ))
    {
        return false;
    }

    StructureChain<vk::PhysicalDeviceFeatures2> supported_features{ m_features };
    physical_device.getDispatcher()->vkGetPhysicalDeviceFeatures2(
        *physical_device, supported_features.root()
    );
    if (!m_features.matches(supported_features.root())) {
        return false;
    }

    return true;
}

auto PhysicalDeviceCapabilities::filter_uncontained_features(
    vk::PhysicalDeviceFeatures& features
) const -> void
{
    remove_physical_device_features(features, m_features.root().features);
}

template <vulkan1x_feature_struct_c Vulkan1XFeatureStruct_T>
auto PhysicalDeviceCapabilities::filter_uncontained_features(
    Vulkan1XFeatureStruct_T& features
) const -> void
{
    if (const util::OptionalRef<const Vulkan1XFeatureStruct_T> contained_features{
            m_features.find<Vulkan1XFeatureStruct_T>() };
        contained_features.has_value())
    {
        remove_physical_device_features(features, *contained_features);
    }
}

template <individual_feature_struct_c FeaturesStruct_T>
auto PhysicalDeviceCapabilities::filter_uncontained_features(
    FeaturesStruct_T& features
) const -> void
{
    if constexpr (core_feature_struct_from_vulkan11_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion11) {
            if (const util::OptionalRef<const vk::PhysicalDeviceVulkan11Features>
                    contained_features{
                        m_features.find<vk::PhysicalDeviceVulkan11Features>() };
                contained_features.has_value())
            {
                remove_physical_device_features(features, *contained_features);
            }
            return;
        }
    }
    else if constexpr (core_feature_struct_from_vulkan12_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion12) {
            if (const util::OptionalRef<const vk::PhysicalDeviceVulkan12Features>
                    contained_features{
                        m_features.find<vk::PhysicalDeviceVulkan12Features>() };
                contained_features.has_value())
            {
                remove_physical_device_features(features, *contained_features);
            }
            return;
        }
    }
    else if constexpr (core_feature_struct_from_vulkan13_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion13) {
            if (const util::OptionalRef<const vk::PhysicalDeviceVulkan13Features>
                    contained_features{
                        m_features.find<vk::PhysicalDeviceVulkan13Features>() };
                contained_features.has_value())
            {
                remove_physical_device_features(features, *contained_features);
            }
            return;
        }
    }
    else if constexpr (core_feature_struct_from_vulkan14_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion14) {
            if (const util::OptionalRef<const vk::PhysicalDeviceVulkan14Features>
                    contained_features{
                        m_features.find<vk::PhysicalDeviceVulkan14Features>() };
                contained_features.has_value())
            {
                remove_physical_device_features(features, *contained_features);
            }
            return;
        }
    }

    if (const util::OptionalRef<const FeaturesStruct_T> contained_features{
            m_features.find<FeaturesStruct_T>() };
        contained_features.has_value())
    {
        remove_physical_device_features(features, *contained_features);
    }
}

auto PhysicalDeviceCapabilities::try_upgrade_api_version(const uint32_t new_api_version)
    -> void
{
    PRECOND(
        new_api_version < vk::makeApiVersion(0, 1, 5, 0),
        "Only Vulkan versions lower than 1.5 are supported"
    );

    if (new_api_version > m_api_version) {
        if (const uint32_t new_minor_version{ vk::apiVersionMinor(new_api_version) };
            new_minor_version > vk::apiVersionMinor(m_api_version))
        {
            if (new_minor_version == 1) {
                upgrade_to_Vulkan11();
            }
            else if (new_minor_version == 2) {
                upgrade_to_Vulkan12();
            }
            else if (new_minor_version == 3) {
                upgrade_to_Vulkan13();
            }
            else if (new_minor_version == 4) {
                upgrade_to_Vulkan14();
            }
        }

        m_api_version = new_api_version;
    }
}

auto PhysicalDeviceCapabilities::insert_extension(const util::StringLiteral extension_name)
    -> void
{
    if (m_api_version >= vk::ApiVersion11) {
        if (vk::PhysicalDeviceVulkan11Features vulkan11_features{};
            try_promote_extension_to_vulkan11(extension_name, vulkan11_features))
        {
            if (vulkan11_features != vk::PhysicalDeviceVulkan11Features{}) {
                insert_features(vulkan11_features);
            }
            return;
        }
    }
    if (m_api_version >= vk::ApiVersion12) {
        if (vk::PhysicalDeviceVulkan12Features vulkan12_features{};
            try_promote_extension_to_vulkan12(extension_name, vulkan12_features))
        {
            if (vulkan12_features != vk::PhysicalDeviceVulkan12Features{}) {
                insert_features(vulkan12_features);
            }
            return;
        }
    }
    if (m_api_version >= vk::ApiVersion13) {
        if (vk::PhysicalDeviceVulkan13Features vulkan13_features{};
            try_promote_extension_to_vulkan13(extension_name, vulkan13_features))
        {
            if (vulkan13_features != vk::PhysicalDeviceVulkan13Features{}) {
                insert_features(vulkan13_features);
            }
            return;
        }
    }
    if (m_api_version >= vk::ApiVersion14) {
        if (vk::PhysicalDeviceVulkan14Features vulkan14_features{};
            try_promote_extension_to_vulkan14(extension_name, vulkan14_features))
        {
            if (vulkan14_features != vk::PhysicalDeviceVulkan14Features{}) {
                insert_features(vulkan14_features);
            }
            return;
        }
    }

    if (!std::ranges::contains(m_extension_names, extension_name)) {
        m_extension_names.push_back(extension_name);
    }
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceFeatures& features
) -> void
{
    merge_physical_device_features(m_features.root().features, features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan11Features& features
) -> void
{
    try_upgrade_api_version(vk::ApiVersion11);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan12Features& features
) -> void
{
    try_upgrade_api_version(vk::ApiVersion12);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan13Features& features
) -> void
{
    try_upgrade_api_version(vk::ApiVersion13);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan14Features& features
) -> void
{
    try_upgrade_api_version(vk::ApiVersion14);

    m_features.merge(features);
}

template <individual_feature_struct_c FeaturesStruct_T>
auto PhysicalDeviceCapabilities::insert_features(const FeaturesStruct_T& features) -> void
{
    if constexpr (core_feature_struct_from_vulkan11_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion11) {
            vk::PhysicalDeviceVulkan11Features comprehensive_features{};
            merge_physical_device_features(comprehensive_features, features);
            m_features.merge(comprehensive_features);
            return;
        }
    }
    else if constexpr (core_feature_struct_from_vulkan12_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion12) {
            vk::PhysicalDeviceVulkan12Features comprehensive_features{};
            merge_physical_device_features(comprehensive_features, features);
            m_features.merge(comprehensive_features);
            return;
        }
    }
    else if constexpr (core_feature_struct_from_vulkan13_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion13) {
            vk::PhysicalDeviceVulkan13Features comprehensive_features{};
            merge_physical_device_features(comprehensive_features, features);
            m_features.merge(comprehensive_features);
            return;
        }
    }
    else if constexpr (core_feature_struct_from_vulkan14_c<FeaturesStruct_T>) {
        if (m_api_version >= vk::ApiVersion14) {
            vk::PhysicalDeviceVulkan14Features comprehensive_features{};
            merge_physical_device_features(comprehensive_features, features);
            m_features.merge(comprehensive_features);
            return;
        }
    }

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    StructureChain<vk::PhysicalDeviceFeatures2> features
) -> void
{
    if (m_api_version < vk::ApiVersion11
        && features.contains<vk::PhysicalDeviceVulkan11Features>())
    {
        upgrade_to_Vulkan11();
    }
    if (m_api_version < vk::ApiVersion12
        && features.contains<vk::PhysicalDeviceVulkan12Features>())
    {
        upgrade_to_Vulkan12();
    }
    if (m_api_version < vk::ApiVersion13
        && features.contains<vk::PhysicalDeviceVulkan13Features>())
    {
        upgrade_to_Vulkan13();
    }
    if (m_api_version < vk::ApiVersion14
        && features.contains<vk::PhysicalDeviceVulkan14Features>())
    {
        upgrade_to_Vulkan14();
    }

    if (m_api_version >= vk::ApiVersion11) {
        vk::PhysicalDeviceVulkan11Features vulkan11_features{};
        features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan11_features);
        features.merge(vulkan11_features);
    }
    if (m_api_version >= vk::ApiVersion12) {
        vk::PhysicalDeviceVulkan12Features vulkan12_features{};
        features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan12_features);
        features.merge(vulkan12_features);
    }
    if (m_api_version >= vk::ApiVersion13) {
        vk::PhysicalDeviceVulkan13Features vulkan13_features{};
        features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan13_features);
        features.merge(vulkan13_features);
    }
    if (m_api_version >= vk::ApiVersion14) {
        vk::PhysicalDeviceVulkan14Features vulkan14_features{};
        features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan14_features);
        features.merge(vulkan14_features);
    }

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::try_remove_extension(
    const util::StringLiteral extension_name
) -> void
{
    if (m_api_version >= vk::ApiVersion11) {
        if (vk::PhysicalDeviceVulkan11Features vulkan11_features{};
            try_promote_extension_to_vulkan11(extension_name, vulkan11_features))
        {
            if (vulkan11_features != vk::PhysicalDeviceVulkan11Features{}) {
                try_remove_features(vulkan11_features);
            }
            return;
        }
    }
    if (m_api_version >= vk::ApiVersion12) {
        if (vk::PhysicalDeviceVulkan12Features vulkan12_features{};
            try_promote_extension_to_vulkan12(extension_name, vulkan12_features))
        {
            if (vulkan12_features != vk::PhysicalDeviceVulkan12Features{}) {
                try_remove_features(vulkan12_features);
            }
            return;
        }
    }
    if (m_api_version >= vk::ApiVersion13) {
        if (vk::PhysicalDeviceVulkan13Features vulkan13_features{};
            try_promote_extension_to_vulkan13(extension_name, vulkan13_features))
        {
            if (vulkan13_features != vk::PhysicalDeviceVulkan13Features{}) {
                try_remove_features(vulkan13_features);
            }
            return;
        }
    }
    if (m_api_version >= vk::ApiVersion14) {
        if (vk::PhysicalDeviceVulkan14Features vulkan14_features{};
            try_promote_extension_to_vulkan14(extension_name, vulkan14_features))
        {
            if (vulkan14_features != vk::PhysicalDeviceVulkan14Features{}) {
                try_remove_features(vulkan14_features);
            }
            return;
        }
    }

    std::erase(m_extension_names, extension_name);
}

auto PhysicalDeviceCapabilities::try_remove_features(
    const vk::PhysicalDeviceFeatures& features
) -> void
{
    m_features.try_remove_features(features);
}

auto PhysicalDeviceCapabilities::try_remove_features(
    const vk::PhysicalDeviceVulkan11Features& features
) -> void
{
    m_features.try_remove_features(features);
}

auto PhysicalDeviceCapabilities::try_remove_features(
    const vk::PhysicalDeviceVulkan12Features& features
) -> void
{
    m_features.try_remove_features(features);
}

auto PhysicalDeviceCapabilities::try_remove_features(
    const vk::PhysicalDeviceVulkan13Features& features
) -> void
{
    m_features.try_remove_features(features);
}

auto PhysicalDeviceCapabilities::try_remove_features(
    const vk::PhysicalDeviceVulkan14Features& features
) -> void
{
    m_features.try_remove_features(features);
}

template <individual_feature_struct_c FeaturesStruct_T>
auto PhysicalDeviceCapabilities::try_remove_features(const FeaturesStruct_T& features)
    -> void
{
    m_features.try_remove_features(features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan11() -> void
{
    PRECOND(m_api_version < vk::ApiVersion11);

    m_api_version = vk::ApiVersion11;

    vk::PhysicalDeviceVulkan11Features vulkan11_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan11_features](const util::StringLiteral extension_name) -> bool {
            return try_promote_extension_to_vulkan11(extension_name, vulkan11_features);
        }
    );
    m_features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan11_features);
    m_features.merge(vulkan11_features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan12() -> void
{
    PRECOND(m_api_version < vk::ApiVersion12);

    if (m_api_version < vk::ApiVersion11) {
        upgrade_to_Vulkan11();
    }

    m_api_version = vk::ApiVersion12;

    vk::PhysicalDeviceVulkan12Features vulkan12_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan12_features](const util::StringLiteral extension_name) -> bool {
            return try_promote_extension_to_vulkan12(extension_name, vulkan12_features);
        }
    );
    m_features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan12_features);
    m_features.merge(vulkan12_features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan13() -> void
{
    PRECOND(m_api_version < vk::ApiVersion13);

    if (m_api_version < vk::ApiVersion12) {
        upgrade_to_Vulkan12();
    }

    m_api_version = vk::ApiVersion13;

    vk::PhysicalDeviceVulkan13Features vulkan13_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan13_features](const util::StringLiteral extension_name) -> bool {
            return try_promote_extension_to_vulkan13(extension_name, vulkan13_features);
        }
    );
    m_features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan13_features);
    m_features.merge(vulkan13_features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan14() -> void
{
    PRECOND(m_api_version < vk::ApiVersion14);

    if (m_api_version < vk::ApiVersion13) {
        upgrade_to_Vulkan13();
    }

    m_api_version = vk::ApiVersion14;

    vk::PhysicalDeviceVulkan14Features vulkan14_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan14_features](const util::StringLiteral extension_name) -> bool {
            return try_promote_extension_to_vulkan14(extension_name, vulkan14_features);
        }
    );
    m_features.remove_and_merge_features_to_vulkan1x_feature_struct(vulkan14_features);
    m_features.merge(vulkan14_features);
}

}   // namespace ddge::vulkan
