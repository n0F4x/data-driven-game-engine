module;

#include <algorithm>
#include <flat_map>
#include <ranges>
#include <utility>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.structure_chains.StructureChain;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.extends_struct_c;
import ddge.modules.vulkan.structure_chains.filter_physical_device_features;
import ddge.modules.vulkan.structure_chains.match_physical_device_features;
import ddge.modules.vulkan.structure_chains.merge_physical_device_features;
import ddge.modules.vulkan.structure_chains.remove_physical_device_features;
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

    template <typename Self_T, extends_struct_c<RootStruct_T> Struct_T>
    constexpr auto operator[](this Self_T&&, std::in_place_type_t<Struct_T>)
        -> util::meta::forward_like_t<Struct_T, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto root_struct(this Self_T&&)
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

    constexpr auto connect() -> void;

    [[nodiscard]]
    constexpr auto matches(
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) const -> bool
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto merge(const vk::PhysicalDeviceFeatures2& physical_device_features)
        -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto remove_unsupported_features(
        const vk::PhysicalDeviceFeatures2& supported_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto remove_features(const vk::PhysicalDeviceFeatures2& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

private:
    RootStruct_T                                   m_root_struct{};
    std::flat_map<vk::StructureType, ErasedStruct> m_chain;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

struct ErasedStructOperations {
    using AddressFunc          = auto(ErasedStruct&) -> void*;
    using TypeFunc             = auto(const ErasedStruct&) -> vk::StructureType;
    using NextPointerFunc      = auto(ErasedStruct&) -> void*&;
    using ConstNextPointerFunc = auto(const ErasedStruct&) -> const void* const&;
    using MatchesFunc = auto(const ErasedStruct&, const vk::BaseOutStructure&) -> bool;
    using MergeFunc   = auto(ErasedStruct&, const vk::PhysicalDeviceFeatures2&) -> void;
    using FilterFunc  = auto(ErasedStruct&, const vk::PhysicalDeviceFeatures2&) -> void;
    using RemoveFunc  = auto(ErasedStruct&, const vk::PhysicalDeviceFeatures2&) -> void;

    std::reference_wrapper<AddressFunc>          address_of;
    std::reference_wrapper<TypeFunc>             type_of;
    std::reference_wrapper<NextPointerFunc>      next_pointer_of;
    std::reference_wrapper<ConstNextPointerFunc> const_next_pointer_of;
    std::add_pointer_t<MatchesFunc>              matches{};
    std::add_pointer_t<MergeFunc>                merge{};
    std::add_pointer_t<FilterFunc>               filter{};
    std::add_pointer_t<FilterFunc>               remove{};
};

template <typename Struct_T>
struct ErasedStructTraits {
    [[nodiscard]]
    constexpr static auto address_of(ErasedStruct& that) -> void*
    {
        return &util::any_cast<Struct_T>(that);
    }

    [[nodiscard]]
    constexpr static auto type_of(const ErasedStruct& that) -> vk::StructureType
    {
        return util::any_cast<Struct_T>(that).sType;
    }

    [[nodiscard]]
    constexpr static auto next_pointer_of(ErasedStruct& that) -> void*&
    {
        return util::any_cast<Struct_T>(that).pNext;
    }

    [[nodiscard]]
    constexpr static auto next_pointer_of(const ErasedStruct& that) -> const void* const&
    {
        return util::any_cast<Struct_T>(that).pNext;
    }

    [[nodiscard]]
    constexpr static auto
        matches(const ErasedStruct& that, const vk::BaseOutStructure& feature_struct)
            -> bool
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        if (feature_struct.sType != Struct_T::structureType) {
            return false;
        }

        return match_physical_device_features(
            util::any_cast<Struct_T>(that),
            reinterpret_cast<const Struct_T&>(feature_struct)
        );
    }

    constexpr static auto merge(
        ErasedStruct&                      that,
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        for (const vk::BaseOutStructure* incoming_feature_struct{
                 static_cast<const vk::BaseOutStructure*>(physical_device_features.pNext) };
             incoming_feature_struct != nullptr;
             incoming_feature_struct = incoming_feature_struct->pNext)
        {
            if (incoming_feature_struct->sType == type_of(that)) {
                merge_physical_device_features(
                    util::any_cast<Struct_T>(that),
                    reinterpret_cast<const Struct_T&>(*incoming_feature_struct)
                );
                return;
            }
        }
    }

    constexpr static auto filter(
        ErasedStruct&                      that,
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        for (const vk::BaseOutStructure* incoming_feature_struct{
                 static_cast<const vk::BaseOutStructure*>(physical_device_features.pNext) };
             incoming_feature_struct != nullptr;
             incoming_feature_struct = incoming_feature_struct->pNext)
        {
            if (incoming_feature_struct->sType == type_of(that)) {
                filter_physical_device_features(
                    util::any_cast<Struct_T>(that),
                    reinterpret_cast<const Struct_T&>(*incoming_feature_struct)
                );
                return;
            }
        }
    }

    constexpr static auto remove(
        ErasedStruct&                      that,
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        for (const vk::BaseOutStructure* incoming_feature_struct{
                 static_cast<const vk::BaseOutStructure*>(physical_device_features.pNext) };
             incoming_feature_struct != nullptr;
             incoming_feature_struct = incoming_feature_struct->pNext)
        {
            if (incoming_feature_struct->sType == type_of(that)) {
                remove_physical_device_features(
                    util::any_cast<Struct_T>(that),
                    reinterpret_cast<const Struct_T&>(*incoming_feature_struct)
                );
                return;
            }
        }
    }

    constexpr static ErasedStructOperations operations{
        .address_of      = address_of,
        .type_of         = type_of,
        .next_pointer_of = static_cast<
            std::add_lvalue_reference_t<ErasedStructOperations::NextPointerFunc>>(
            next_pointer_of
        ),
        .const_next_pointer_of = static_cast<
            std::add_lvalue_reference_t<ErasedStructOperations::ConstNextPointerFunc>>(
            next_pointer_of
        ),
        .matches = extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2> ? matches
                                                                           : nullptr,
        .merge = extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2> ? merge : nullptr,
        .filter = extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2> ? filter
                                                                          : nullptr,
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
    constexpr auto type() const -> vk::StructureType
    {
        return m_operations.get().type_of(*this);
    }

    [[nodiscard]]
    constexpr auto next_pointer() -> void*&
    {
        return m_operations.get().next_pointer_of(*this);
    }

    [[nodiscard]]
    constexpr auto next_pointer() const -> const void* const&
    {
        return m_operations.get().const_next_pointer_of(*this);
    }

    [[nodiscard]]
    constexpr auto matches(const vk::BaseOutStructure& feature_struct) const -> bool
    {
        PRECOND(m_operations.get().matches != nullptr);
        return m_operations.get().matches(*this, feature_struct);
    }

    constexpr auto merge(const vk::PhysicalDeviceFeatures2& physical_device_features)
        -> void
    {
        PRECOND(m_operations.get().merge != nullptr);
        m_operations.get().merge(*this, physical_device_features);
    }

    constexpr auto filter(const vk::PhysicalDeviceFeatures2& physical_device_features)
        -> void
    {
        PRECOND(m_operations.get().filter != nullptr);
        m_operations.get().filter(*this, physical_device_features);
    }

    constexpr auto remove(const vk::PhysicalDeviceFeatures2& physical_device_features)
        -> void
    {
        PRECOND(m_operations.get().remove != nullptr);
        m_operations.get().remove(*this, physical_device_features);
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
template <typename Self_T, extends_struct_c<RootStruct_T> Struct_T>
constexpr auto StructureChain<RootStruct_T>::operator[](
    this Self_T&& self,
    std::in_place_type_t<Struct_T>
) -> util::meta::forward_like_t<Struct_T, Self_T>
{
    const auto [iter, inserted] = self.m_chain.try_emplace(
        RootStruct_T::structureType, std::in_place_type<Struct_T>
    );

    if (inserted) {
        self.connect();
    }

    return util::any_cast<Struct_T>(std::forward_like<Self_T>(iter->second));
}

template <util::meta::naked_c RootStruct_T>
template <typename Self_T>
constexpr auto StructureChain<RootStruct_T>::root_struct(this Self_T&& self)
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

    return util::any_cast<Struct_T>(*iter);
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
constexpr auto StructureChain<RootStruct_T>::connect() -> void
{
    for (std::reference_wrapper<void*> previous_pointer{ m_root_struct.pNext };
         ErasedStruct& next_struct : m_chain | std::views::values)
    {
        previous_pointer.get() = next_struct.address();
        previous_pointer       = next_struct.next_pointer();
    }
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

    for (const ErasedStruct& erased_struct : m_chain | std::views::values) {
        bool matches_struct{};
        for (const vk::BaseOutStructure* supported_feature_struct{
                 static_cast<const vk::BaseOutStructure*>(physical_device_features.pNext) };
             supported_feature_struct != nullptr;
             supported_feature_struct = supported_feature_struct->pNext)
        {
            if (erased_struct.matches(*supported_feature_struct)) {
                matches_struct = true;
                break;
            }
        }
        if (!matches_struct) {
            return false;
        }
    }

    return true;
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::merge(
    const vk::PhysicalDeviceFeatures2& physical_device_features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    merge_physical_device_features(
        m_root_struct.features, physical_device_features.features
    );

    for (ErasedStruct& erased_struct : m_chain | std::views::values) {
        erased_struct.merge(physical_device_features);
    }
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::remove_unsupported_features(
    const vk::PhysicalDeviceFeatures2& supported_features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    filter_physical_device_features(m_root_struct.features, supported_features.features);

    for (ErasedStruct& erased_struct : m_chain | std::views::values) {
        erased_struct.filter(supported_features);
    }
}

template <util::meta::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::remove_features(
    const vk::PhysicalDeviceFeatures2& features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    remove_physical_device_features(m_root_struct.features, features.features);

    for (ErasedStruct& erased_struct : m_chain | std::views::values) {
        erased_struct.remove(features);
    }
}

}   // namespace ddge::vulkan
