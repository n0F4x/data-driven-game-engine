module;

#include <format>
#include <optional>
#include <typeindex>
#include <utility>

#include <tsl/ordered_map.h>

#include "utility/contracts_macros.hpp"

export module core.store.Store;

import core.store.item_c;
import core.store.Policy;

import utility.containers.Any;
import utility.containers.MoveOnlyAny;
import utility.containers.OptionalRef;
import utility.contracts;
import utility.meta.reflection.name_of;
import utility.meta.type_traits.const_like;

namespace core::store {

export template <Policy policy_T = Policy::eDefault>
class BasicStore {
public:
    BasicStore()                      = default;
    BasicStore(const BasicStore&)     = delete;
    BasicStore(BasicStore&&) noexcept = default;
    ~BasicStore();

    auto operator=(const BasicStore&) -> BasicStore&     = delete;
    auto operator=(BasicStore&&) noexcept -> BasicStore& = default;

    template <item_c Item_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Item_T&;

    template <typename Item_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::meta::const_like_t<Item_T, Self_T>>;

    template <typename Item_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) -> util::meta::const_like_t<Item_T, Self_T>&;

    template <typename Item_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    using Any = std::conditional_t<
        policy_T == Policy::eMoveOnlyItems,
        util::BasicMoveOnlyAny<0>,
        util::BasicAny<0>>;

    tsl::ordered_map<std::type_index, Any> m_map;
};

export using Store = BasicStore<>;

}   // namespace core::store

template <core::store::Policy policy_T>
template <core::store::item_c Item_T, typename... Args_T>
auto core::store::BasicStore<policy_T>::emplace(Args_T&&... args) -> Item_T&
{
    return util::any_cast<Item_T>(
        m_map
            .try_emplace(
                typeid(Item_T), std::in_place_type<Item_T>, std::forward<Args_T>(args)...

            )
            .first.value()
    );
}

template <core::store::Policy policy_T>
template <typename Item_T, typename Self_T>
auto core::store::BasicStore<policy_T>::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Item_T, Self_T>>
{
    const auto iter{ self.m_map.find(typeid(Item_T)) };
    if (iter == self.m_map.cend()) {
        return std::nullopt;
    }

    return util::any_cast<Item_T>(*iter);
}

template <core::store::Policy policy_T>
template <typename Item_T, typename Self_T>
auto core::store::BasicStore<policy_T>::at(this Self_T& self)
    -> util::meta::const_like_t<Item_T, Self_T>&
{
    PRECOND(
        self.template contains<Item_T>(),
        std::format("Item {} not found", util::meta::name_of<Item_T>())
    );
    return util::any_cast<Item_T>(self.m_map.at(typeid(Item_T)));
}

template <core::store::Policy policy_T>
template <typename Item_T>
auto core::store::BasicStore<policy_T>::contains() const noexcept -> bool
{
    return m_map.contains(typeid(Item_T));
}

template <core::store::Policy policy_T>
core::store::BasicStore<policy_T>::~BasicStore<policy_T>()
{
    while (!m_map.empty()) {
        m_map.pop_back();
    }
}
