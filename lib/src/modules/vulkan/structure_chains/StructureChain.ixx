module;

#include <algorithm>
#include <flat_map>
#include <ranges>
#include <utility>
#include <vector>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.structure_chains.StructureChain;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.core_feature_struct_from_vulkan1x_c;
import ddge.modules.vulkan.structure_chains.erase_physical_device_features;
import ddge.modules.vulkan.structure_chains.extends_struct_c;
import ddge.modules.vulkan.structure_chains.filter_physical_device_features;
import ddge.modules.vulkan.structure_chains.is_empty_feature_sruct;
import ddge.modules.vulkan.structure_chains.match_physical_device_features;
import ddge.modules.vulkan.structure_chains.merge_physical_device_features;
import ddge.modules.vulkan.structure_chains.vulkan1x_feature_struct_c;
import ddge.utility.any_cast;
import ddge.utility.containers.AnyCopyable;
import ddge.utility.containers.OptionalRef;
import ddge.utility.contracts;
import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.type_traits.const_like;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::vulkan {

class ErasedStruct;

export template <util::meta::naked_c RootStruct_T>
class StructureChain {
public:
    StructureChain() = default;
    StructureChain(const StructureChain&);
    StructureChain(StructureChain&&) noexcept;

    auto operator=(const StructureChain&) -> StructureChain&;
    auto operator=(StructureChain&&) noexcept -> StructureChain&;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto root(this Self_T&&)
        -> util::meta::forward_like_t<RootStruct_T, Self_T>;

    template <extends_struct_c<RootStruct_T> Struct_T>
    [[nodiscard]]
    constexpr auto contains() const -> bool;
    template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
    [[nodiscard]]
    constexpr auto find(this Self_T&) -> util::
        OptionalRef<util::meta::const_like_t<Struct_T, std::remove_reference_t<Self_T>>>;
    template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
    [[nodiscard]]
    constexpr auto at(this Self_T&&) -> util::meta::forward_like_t<Struct_T, Self_T>;

    [[nodiscard]]
    constexpr auto matches(
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) const -> bool
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
    constexpr auto merge(const Features_T& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;
    constexpr auto merge(const StructureChain<vk::PhysicalDeviceFeatures2>& other) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    template <vulkan1x_feature_struct_c Vulkan1XFeatures_T>
    constexpr auto erase_and_merge_features_to_vulkan1x_feature_struct(
        Vulkan1XFeatures_T& vulkan1X_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto erase_unsupported_features(
        const vk::PhysicalDeviceFeatures2& supported_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto erase_features(const vk::PhysicalDeviceFeatures& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;
    template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
    constexpr auto erase_features(const Features_T& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;
    constexpr auto erase_features(
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

private:
    RootStruct_T                                   m_root_struct{};
    std::flat_map<vk::StructureType, ErasedStruct> m_chain;

    template <typename Self_T, extends_struct_c<RootStruct_T> Struct_T>
    constexpr auto operator[](this Self_T&&, std::in_place_type_t<Struct_T>)
        -> util::meta::forward_like_t<Struct_T, Self_T>;

    constexpr auto connect() -> void;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

struct ErasedStructOperations {
    using AddressFunc     = auto(ErasedStruct&) -> void*;
    using NextPointerFunc = auto(ErasedStruct&) -> void*&;
    using EmptyFunc       = auto(const ErasedStruct&) -> bool;
    using MatchesFunc     = auto(const ErasedStruct&, const vk::BaseInStructure&) -> bool;
    using MergeFunc       = auto(ErasedStruct&, const vk::BaseInStructure&) -> void;
    using MergeToVulkan11FeaturesFunc =
        auto(const ErasedStruct&, vk::PhysicalDeviceVulkan11Features&) -> void;
    using MergeToVulkan12FeaturesFunc =
        auto(const ErasedStruct&, vk::PhysicalDeviceVulkan12Features&) -> void;
    using MergeToVulkan13FeaturesFunc =
        auto(const ErasedStruct&, vk::PhysicalDeviceVulkan13Features&) -> void;
    using MergeToVulkan14FeaturesFunc =
        auto(const ErasedStruct&, vk::PhysicalDeviceVulkan14Features&) -> void;
    using FilterFunc = auto(ErasedStruct&, const vk::BaseInStructure&) -> void;
    using RemoveFunc = auto(ErasedStruct&, const vk::BaseInStructure&) -> void;

    std::reference_wrapper<AddressFunc>             address_of;
    std::reference_wrapper<NextPointerFunc>         next_pointer_of;
    std::add_pointer_t<EmptyFunc>                   empty{};
    std::add_pointer_t<MatchesFunc>                 matches{};
    std::add_pointer_t<MergeFunc>                   merge{};
    std::add_pointer_t<MergeToVulkan11FeaturesFunc> merge_to_Vulkan11_features{};
    std::add_pointer_t<MergeToVulkan12FeaturesFunc> merge_to_Vulkan12_features{};
    std::add_pointer_t<MergeToVulkan13FeaturesFunc> merge_to_Vulkan13_features{};
    std::add_pointer_t<MergeToVulkan14FeaturesFunc> merge_to_Vulkan14_features{};
    std::add_pointer_t<FilterFunc>                  filter{};
    std::add_pointer_t<RemoveFunc>                  erase{};
};

template <typename Struct_T>
struct ErasedStructTraits {
    [[nodiscard]]
    constexpr static auto address_of(ErasedStruct& that) -> void*
    {
        return &util::any_cast<Struct_T>(that);
    }

    [[nodiscard]]
    constexpr static auto next_pointer_of(ErasedStruct& that) -> void*&
    {
        return util::any_cast<Struct_T>(that).pNext;
    }

    [[nodiscard]]
    constexpr static auto empty(const ErasedStruct& that) -> bool
    {
        return is_empty_feature_sruct(util::any_cast<Struct_T>(that));
    }

    [[nodiscard]]
    constexpr static auto
        matches(const ErasedStruct& that, const vk::BaseInStructure& other) -> bool
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        return match_physical_device_features(
            util::any_cast<Struct_T>(that), reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static auto merge(ErasedStruct& that, const vk::BaseInStructure& other)
        -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        merge_physical_device_features(
            util::any_cast<Struct_T>(that), reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static auto merge_to_Vulkan11_features(
        const ErasedStruct&                 that,
        vk::PhysicalDeviceVulkan11Features& vulkan11_features
    ) -> void
        requires core_feature_struct_from_vulkan11_c<Struct_T>
    {
        merge_physical_device_features(vulkan11_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto merge_to_Vulkan12_features(
        const ErasedStruct&                 that,
        vk::PhysicalDeviceVulkan12Features& vulkan12_features
    ) -> void
        requires core_feature_struct_from_vulkan12_c<Struct_T>
    {
        merge_physical_device_features(vulkan12_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto merge_to_Vulkan13_features(
        const ErasedStruct&                 that,
        vk::PhysicalDeviceVulkan13Features& vulkan13_features
    ) -> void
        requires core_feature_struct_from_vulkan13_c<Struct_T>
    {
        merge_physical_device_features(vulkan13_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto merge_to_Vulkan14_features(
        const ErasedStruct&                 that,
        vk::PhysicalDeviceVulkan14Features& vulkan14_features
    ) -> void
        requires core_feature_struct_from_vulkan14_c<Struct_T>
    {
        merge_physical_device_features(vulkan14_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto filter(ErasedStruct& that, const vk::BaseInStructure& other)
        -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        filter_physical_device_features(
            util::any_cast<Struct_T>(that), reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static auto erase(ErasedStruct& that, const vk::BaseInStructure& other)
        -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        erase_physical_device_features(
            util::any_cast<Struct_T>(that), reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static ErasedStructOperations operations{
        .address_of      = address_of,
        .next_pointer_of = next_pointer_of,
        .empty =
            [] {
                if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>) {
                    return empty;
                }
                else {
                    return nullptr;
                }
            }(),
        .matches =
            [] {
                if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>) {
                    return matches;
                }
                else {
                    return nullptr;
                }
            }(),
        .merge =
            [] {
                if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>) {
                    return merge;
                }
                else {
                    return nullptr;
                }
            }(),
        .merge_to_Vulkan11_features =
            [] {
                if constexpr (core_feature_struct_from_vulkan11_c<Struct_T>) {
                    return merge_to_Vulkan11_features;
                }
                else {
                    return nullptr;
                }
            }(),
        .merge_to_Vulkan12_features =
            [] {
                if constexpr (core_feature_struct_from_vulkan12_c<Struct_T>) {
                    return merge_to_Vulkan12_features;
                }
                else {
                    return nullptr;
                }
            }(),
        .merge_to_Vulkan13_features =
            [] {
                if constexpr (core_feature_struct_from_vulkan13_c<Struct_T>) {
                    return merge_to_Vulkan13_features;
                }
                else {
                    return nullptr;
                }
            }(),
        .merge_to_Vulkan14_features =
            [] {
                if constexpr (core_feature_struct_from_vulkan14_c<Struct_T>) {
                    return merge_to_Vulkan14_features;
                }
                else {
                    return nullptr;
                }
            }(),
        .filter =
            [] {
                if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>) {
                    return filter;
                }
                else {
                    return nullptr;
                }
            }(),
        .erase =
            [] {
                if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>) {
                    return erase;
                }
                else {
                    return nullptr;
                }
            }(),
    };
};

class ErasedStruct : public util::AnyCopyable {
public:
    template <typename Struct_T>
    explicit ErasedStruct(std::in_place_type_t<Struct_T>)
        : util::AnyCopyable{ std::in_place_type<Struct_T> },
          m_operations{ ErasedStructTraits<Struct_T>::operations }
    {}

    template <typename Struct_T>
    explicit ErasedStruct(const Struct_T& real_struct)
        : util::AnyCopyable{ real_struct },
          m_operations{ ErasedStructTraits<Struct_T>::operations }
    {}

    [[nodiscard]]
    constexpr auto address() -> void*
    {
        return m_operations.get().address_of(*this);
    }

    [[nodiscard]]
    constexpr auto next_pointer() -> void*&
    {
        return m_operations.get().next_pointer_of(*this);
    }

    [[nodiscard]]
    constexpr auto empty() const -> bool
    {
        PRECOND(m_operations.get().empty != nullptr);
        return m_operations.get().empty(*this);
    }

    [[nodiscard]]
    constexpr auto matches(const vk::BaseInStructure& feature_struct) const -> bool
    {
        PRECOND(m_operations.get().matches != nullptr);
        return m_operations.get().matches(*this, feature_struct);
    }

    constexpr auto merge(const vk::BaseInStructure& features) -> void
    {
        PRECOND(m_operations.get().merge != nullptr);
        m_operations.get().merge(*this, features);
    }

    constexpr auto try_merge_to_Vulkan11_features(
        vk::PhysicalDeviceVulkan11Features& vulkan11_features
    ) const -> bool
    {
        if (m_operations.get().merge_to_Vulkan11_features != nullptr) {
            m_operations.get().merge_to_Vulkan11_features(*this, vulkan11_features);
            return true;
        }
        return false;
    }

    constexpr auto try_merge_to_Vulkan12_features(
        vk::PhysicalDeviceVulkan12Features& vulkan12_features
    ) const -> bool
    {
        if (m_operations.get().merge_to_Vulkan12_features != nullptr) {
            m_operations.get().merge_to_Vulkan12_features(*this, vulkan12_features);
            return true;
        }
        return false;
    }

    constexpr auto try_merge_to_Vulkan13_features(
        vk::PhysicalDeviceVulkan13Features& vulkan13_features
    ) const -> bool
    {
        if (m_operations.get().merge_to_Vulkan13_features != nullptr) {
            m_operations.get().merge_to_Vulkan13_features(*this, vulkan13_features);
            return true;
        }
        return false;
    }

    constexpr auto try_merge_to_Vulkan14_features(
        vk::PhysicalDeviceVulkan14Features& vulkan14_features
    ) const -> bool
    {
        if (m_operations.get().merge_to_Vulkan14_features != nullptr) {
            m_operations.get().merge_to_Vulkan14_features(*this, vulkan14_features);
            return true;
        }
        return false;
    }

    constexpr auto filter(const vk::BaseInStructure& features) -> void
    {
        PRECOND(m_operations.get().filter != nullptr);
        m_operations.get().filter(*this, features);
    }

    constexpr auto erase(const vk::BaseInStructure& features) -> void
    {
        PRECOND(m_operations.get().erase != nullptr);
        m_operations.get().erase(*this, features);
    }

private:
    std::reference_wrapper<const ErasedStructOperations> m_operations;
};

template <util::meta::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(const StructureChain& other)
    : m_root_struct{ other.m_root_struct },
      m_chain{ other.m_chain }
{
    connect();
}

template <util::meta::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(StructureChain&& other) noexcept
    : m_root_struct{ std::move(other.m_root_struct) },
      m_chain{ std::move(other.m_chain) }
{
    connect();
}

template <util::meta::naked_c RootStruct_T>
auto StructureChain<RootStruct_T>::operator=(const StructureChain& other)
    -> StructureChain&
{
    if (this == &other) {
        return *this;
    }

    m_root_struct = other.m_root_struct;
    m_chain       = other.m_chain;

    connect();

    return *this;
}

template <util::meta::naked_c RootStruct_T>
auto StructureChain<RootStruct_T>::operator=(StructureChain&& other) noexcept
    -> StructureChain&
{
    if (this == &other) {
        return *this;
    }

    m_root_struct = std::move(other.m_root_struct);
    m_chain       = std::move(other.m_chain);

    connect();

    return *this;
}

template <util::meta::naked_c RootStruct_T>
template <typename Self_T>
constexpr auto StructureChain<RootStruct_T>::root(this Self_T&& self)
    -> util::meta::forward_like_t<RootStruct_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_root_struct);
}

template <util::meta::naked_c RootStruct_T>
template <extends_struct_c<RootStruct_T> Struct_T>
constexpr auto StructureChain<RootStruct_T>::contains() const -> bool
{
    return m_chain.contains(Struct_T::structureType);
}

template <util::meta::naked_c RootStruct_T>
template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
constexpr auto StructureChain<RootStruct_T>::find(
    this Self_T& self
) -> util::OptionalRef<util::meta::const_like_t<Struct_T, std::remove_reference_t<Self_T>>>
{
    const auto iter{ self.m_chain.find(Struct_T::structureType) };
    if (iter == self.m_chain.cend()) {
        return std::nullopt;
    }

    return util::any_cast<Struct_T>(iter->second);
}

template <util::meta::naked_c RootStruct_T>
template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
constexpr auto StructureChain<RootStruct_T>::at(this Self_T&& self)
    -> util::meta::forward_like_t<Struct_T, Self_T>
{
    PRECOND(self.template contains<Struct_T>());

    return util::any_cast<Struct_T>(
        std::forward_like<Self_T>(self.m_map.at(Struct_T::structureType))
    );
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::matches(
    const vk::PhysicalDeviceFeatures2& physical_device_features
) const -> bool
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    if (!match_physical_device_features(
            m_root_struct.features, physical_device_features.features
        ))
    {
        return false;
    }

    for (const vk::BaseInStructure* supported_feature_struct{
             static_cast<const vk::BaseInStructure*>(physical_device_features.pNext) };
         supported_feature_struct != nullptr;
         supported_feature_struct = supported_feature_struct->pNext)
    {
        if (const auto iter = m_chain.find(supported_feature_struct->sType);
            iter != m_chain.cend() && !iter->second.matches(*supported_feature_struct))
        {
            return false;
        }
    }

    return true;
}

template <util::meta::naked_c RootStruct_T>
template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
constexpr auto StructureChain<RootStruct_T>::merge(const Features_T& features) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    merge_physical_device_features(operator[](std::in_place_type<Features_T>), features);
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::merge(
    const StructureChain<vk::PhysicalDeviceFeatures2>& other
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    merge_physical_device_features(m_root_struct.features, other.m_root_struct.features);

    bool needs_reconnect{};
    for (const auto& [incoming_sType, incoming_features] : other.m_chain) {
        const auto [iter, inserted] =
            m_chain.try_emplace(incoming_sType, incoming_features);

        iter->second.merge(
            util::reinterpret_any_cast<const vk::BaseInStructure&>(incoming_features)
        );

        needs_reconnect |= inserted;
    }

    if (needs_reconnect) {
        connect();
    }
}

template <util::meta::naked_c RootStruct_T>
template <vulkan1x_feature_struct_c Vulkan1XFeatures_T>
constexpr auto
    StructureChain<RootStruct_T>::erase_and_merge_features_to_vulkan1x_feature_struct(
        Vulkan1XFeatures_T& vulkan1X_features
    ) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    std::vector<vk::StructureType> marked_for_removal;

    for (const auto [sType, erased_features] : m_chain) {
        if constexpr (std::is_same_v<Vulkan1XFeatures_T, vk::PhysicalDeviceVulkan11Features>)
        {
            if (erased_features.try_merge_to_Vulkan11_features(vulkan1X_features)) {
                marked_for_removal.push_back(sType);
            }
        }
        else if constexpr (std::is_same_v<
                               Vulkan1XFeatures_T,
                               vk::PhysicalDeviceVulkan12Features>)
        {
            if (erased_features.try_merge_to_Vulkan12_features(vulkan1X_features)) {
                marked_for_removal.push_back(sType);
            }
        }
        else if constexpr (std::is_same_v<
                               Vulkan1XFeatures_T,
                               vk::PhysicalDeviceVulkan13Features>)
        {
            if (erased_features.try_merge_to_Vulkan13_features(vulkan1X_features)) {
                marked_for_removal.push_back(sType);
            }
        }
        else if constexpr (std::is_same_v<
                               Vulkan1XFeatures_T,
                               vk::PhysicalDeviceVulkan14Features>)
        {
            if (erased_features.try_merge_to_Vulkan14_features(vulkan1X_features)) {
                marked_for_removal.push_back(sType);
            }
        }
        else {
            static_assert(false, "Vulkan 1.5 or higher is not supported");
        }
    }

    for (const vk::StructureType sType : marked_for_removal) {
        m_chain.erase(sType);
    }

    if (!marked_for_removal.empty()) {
        connect();
    }
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::erase_unsupported_features(
    const vk::PhysicalDeviceFeatures2& supported_features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    filter_physical_device_features(m_root_struct.features, supported_features.features);

    bool needs_reconnect{};

    for (const vk::BaseInStructure* incoming_feature_struct{
             static_cast<const vk::BaseInStructure*>(supported_features.pNext) };
         incoming_feature_struct != nullptr;
         incoming_feature_struct = incoming_feature_struct->pNext)
    {
        if (const auto iter = m_chain.find(incoming_feature_struct->sType);
            iter != m_chain.cend())
        {
            iter->second.filter(*incoming_feature_struct);
            if (iter->second.empty()) {
                m_chain.erase(iter);
                needs_reconnect |= true;
            }
        }
    }

    if (needs_reconnect) {
        connect();
    }
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::erase_features(
    const vk::PhysicalDeviceFeatures& features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    erase_physical_device_features(m_root_struct.features, features);
}

template <util::meta::naked_c RootStruct_T>
template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
constexpr auto StructureChain<RootStruct_T>::erase_features(const Features_T& features)
    -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    if (const auto iter = m_chain.find(Features_T::structureType); iter != m_chain.cend())
    {
        iter->second.erase(reinterpret_cast<const vk::BaseInStructure&>(features));
        if (iter->second.empty()) {
            m_chain.erase(iter);
            connect();
        }
    }
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::erase_features(
    const vk::PhysicalDeviceFeatures2& physical_device_features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    erase_physical_device_features(
        m_root_struct.features, physical_device_features.features
    );

    bool needs_reconnect{};

    for (const vk::BaseInStructure* incoming_feature_struct{
             static_cast<const vk::BaseInStructure*>(physical_device_features.pNext) };
         incoming_feature_struct != nullptr;
         incoming_feature_struct = incoming_feature_struct->pNext)
    {
        if (const auto iter = m_chain.find(incoming_feature_struct->sType);
            iter != m_chain.cend())
        {
            iter->second.erase(*incoming_feature_struct);
            if (iter->second.empty()) {
                m_chain.erase(iter);
                needs_reconnect |= true;
            }
        }
    }

    if (needs_reconnect) {
        connect();
    }
}

template <util::meta::naked_c RootStruct_T>
template <typename Self_T, extends_struct_c<RootStruct_T> Struct_T>
constexpr auto StructureChain<RootStruct_T>::operator[](
    this Self_T&& self,
    std::in_place_type_t<Struct_T>
) -> util::meta::forward_like_t<Struct_T, Self_T>
{
    const auto [iter, inserted] =
        self.m_chain.try_emplace(Struct_T::structureType, std::in_place_type<Struct_T>);

    if (inserted) {
        self.connect();
    }

    return util::any_cast<Struct_T>(std::forward_like<Self_T>(iter->second));
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::connect() -> void
{
    std::reference_wrapper<void*> previous_pointer{ m_root_struct.pNext };
    for (ErasedStruct& next_struct : m_chain | std::views::values) {
        previous_pointer.get() = next_struct.address();
        previous_pointer       = next_struct.next_pointer();
    }
    previous_pointer.get() = nullptr;
}

}   // namespace ddge::vulkan
