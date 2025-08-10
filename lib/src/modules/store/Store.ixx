module;

#include <format>
#include <optional>
#include <typeindex>
#include <utility>

#include <tsl/ordered_map.h>

#include "utility/contracts_macros.hpp"

export module ddge.modules.store.Store;

import ddge.modules.store.item_c;

import ddge.utility.containers.MoveOnlyAny;
import ddge.utility.containers.OptionalRef;
import ddge.utility.contracts;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.type_traits.const_like;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::store {

export class Store {
public:
    Store()                 = default;
    Store(const Store&)     = delete;
    Store(Store&&) noexcept = default;
    ~Store();

    auto operator=(const Store&) -> Store&     = delete;
    auto operator=(Store&&) noexcept -> Store& = default;

    template <item_c Item_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Item_T&;

    template <item_c Item_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::meta::const_like_t<Item_T, Self_T>>;

    template <item_c Item_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::meta::forward_like_t<Item_T, Self_T>;

    template <item_c Item_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    tsl::ordered_map<std::type_index, util::BasicMoveOnlyAny<0>> m_map;
};

}   // namespace ddge::store

template <ddge::store::item_c Item_T, typename... Args_T>
auto ddge::store::Store::emplace(Args_T&&... args) -> Item_T&
{
    return util::any_cast<Item_T>(
        m_map
            .try_emplace(
                typeid(Item_T), std::in_place_type<Item_T>, std::forward<Args_T>(args)...

            )
            .first.value()
    );
}

template <ddge::store::item_c Item_T, typename Self_T>
auto ddge::store::Store::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Item_T, Self_T>>
{
    const auto iter{ self.m_map.find(typeid(Item_T)) };
    if (iter == self.m_map.cend()) {
        return std::nullopt;
    }

    return util::any_cast<Item_T>(*iter);
}

template <ddge::store::item_c Item_T, typename Self_T>
auto ddge::store::Store::at(this Self_T&& self)
    -> util::meta::forward_like_t<Item_T, Self_T>
{
    PRECOND(
        self.template contains<Item_T>(),
        std::format("Item {} not found", util::meta::name_of<Item_T>())
    );

    return util::any_cast<Item_T>(   //
        std::forward_like<Self_T>(self.m_map.at(typeid(Item_T)))
    );
}

template <ddge::store::item_c Item_T>
auto ddge::store::Store::contains() const noexcept -> bool
{
    return m_map.contains(typeid(Item_T));
}

module :private;

ddge::store::Store::~Store()
{
    while (!m_map.empty()) {
        m_map.pop_back();
    }
}
