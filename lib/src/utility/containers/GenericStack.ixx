module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <deque>
#include <format>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "utility/contract_macros.hpp"

export module ddge.utility.containers.GenericStack;

import ddge.utility.containers.MoveOnlyAny;
import ddge.utility.containers.OptionalRef;
import ddge.utility.contracts;
import ddge.utility.meta.reflection.hash;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.type_traits.const_like;
import ddge.utility.ScopeFail;

namespace ddge::util {

export template <typename T, typename Any_T>
concept basic_generic_stack_item_c = storable_in_any_c<T, Any_T>;

export template <typename T, typename Any_T>
concept decays_to_basic_generic_stack_item_c =
    basic_generic_stack_item_c<std::decay_t<T>, Any_T>;

/*
 * References to contained items are valid until the instance is alive.
 * Destroying a "moved-from" instance does not invalidate any references to items.
 */
export template <move_only_any_c Any_T = BasicMoveOnlyAny<0>>
    requires(Any_T::size() == 0)
class BasicGenericStack {
public:
    using Any = Any_T;

    // required for interfacing with the standard
    using allocator_type = std::pmr::polymorphic_allocator<>;


    BasicGenericStack() = default;
    explicit BasicGenericStack(const allocator_type& allocator);
    BasicGenericStack(const BasicGenericStack&)     = delete;
    BasicGenericStack(BasicGenericStack&&) noexcept = default;
    BasicGenericStack(BasicGenericStack&&, const allocator_type& allocator);
    ~BasicGenericStack();

    auto operator=(const BasicGenericStack&) -> BasicGenericStack&     = delete;
    auto operator=(BasicGenericStack&&) noexcept -> BasicGenericStack& = default;


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;


    [[nodiscard]]
    auto empty() const noexcept -> bool;

    template <basic_generic_stack_item_c<Any_T> Item_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept -> OptionalRef<meta::const_like_t<Item_T, Self_T>>;

    template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) -> meta::const_like_t<Item_T, Self_T>&;


    template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
    auto insert(Item_T&& item) -> Item_T&;
    template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
    auto try_insert(Item_T&& item) -> std::pair<Item_T&, bool>;
    template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
        requires(std::constructible_from<Item_T, Args_T && ...>)
    auto emplace(Args_T&&... args) -> Item_T&;
    template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
        requires(std::constructible_from<Item_T, Args_T && ...>)
    auto try_emplace(Args_T&&... args) -> std::pair<Item_T&, bool>;

private:
    std::pmr::deque<uint64_t> m_type_hashes;
    std::pmr::deque<Any>      m_items;
};

export using GenericStack = BasicGenericStack<>;

export template <typename T>
concept generic_stack_item_c = basic_generic_stack_item_c<T, GenericStack::Any>;

export template <typename T>
concept decays_to_generic_stack_item_c = generic_stack_item_c<std::decay_t<T>>;

}   // namespace ddge::util

namespace ddge::util {

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
BasicGenericStack<Any_T>::BasicGenericStack(const allocator_type& allocator)
    : m_type_hashes{ allocator },
      m_items{ allocator }
{}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
BasicGenericStack<Any_T>::BasicGenericStack(
    BasicGenericStack&&   other,
    const allocator_type& allocator
)
    : m_type_hashes{ std::move(other.m_type_hashes), allocator },
      m_items{ std::move(other.m_items), allocator }
{}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
BasicGenericStack<Any_T>::~BasicGenericStack()
{
    while (!m_items.empty()) {
        m_items.pop_back();
    }
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
auto BasicGenericStack<Any_T>::get_allocator() const noexcept -> allocator_type
{
    return m_items.get_allocator();
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
auto BasicGenericStack<Any_T>::empty() const noexcept -> bool
{
    return m_items.empty();
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::contains() const noexcept -> bool
{
    return std::ranges::contains(m_type_hashes, meta::hash<Item_T>());
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
auto BasicGenericStack<Any_T>::find(this Self_T& self) noexcept
    -> OptionalRef<meta::const_like_t<Item_T, Self_T>>
{
    const auto hash_iter =
        std::ranges::find(self.BasicGenericStack::m_type_hashes, meta::hash<Item_T>());
    if (hash_iter == self.BasicGenericStack::m_type_hashes.cend()) {
        return std::nullopt;
    }

    return OptionalRef<meta::const_like_t<Item_T, Self_T>>{
        any_cast<Item_T>(
            *std::next(
                self.BasicGenericStack::m_items.begin(),
                std::distance(self.BasicGenericStack::m_type_hashes.begin(), hash_iter)
            )   //
        ),
    };
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename Self_T>
auto BasicGenericStack<Any_T>::at(this Self_T& self)
    -> meta::const_like_t<Item_T, Self_T>&
{
    const OptionalRef found_item{ self.BasicGenericStack::template find<Item_T>() };

    PRECOND(
        found_item.has_value(),
        std::format("Item of type `{}` not found", meta::name_of<Item_T>())
    );

    return *found_item;
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::insert(Item_T&& item) -> Item_T&
{
    return emplace<std::decay_t<Item_T>>(std::forward<Item_T>(item));
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <decays_to_basic_generic_stack_item_c<Any_T> Item_T>
auto BasicGenericStack<Any_T>::try_insert(Item_T&& item) -> std::pair<Item_T&, bool>
{
    return try_emplace<std::decay_t<Item_T>>(std::forward<Item_T>(item));
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
    requires(std::constructible_from<Item_T, Args_T && ...>)
auto BasicGenericStack<Any_T>::emplace(Args_T&&... args) -> Item_T&
{
    auto&& [item, success]{ try_emplace<Item_T>(std::forward<Args_T>(args)...) };
    PRECOND(success);
    return item;
}

template <move_only_any_c Any_T>
    requires(Any_T::size() == 0)
template <basic_generic_stack_item_c<Any_T> Item_T, typename... Args_T>
    requires(std::constructible_from<Item_T, Args_T && ...>)
auto BasicGenericStack<Any_T>::try_emplace(Args_T&&... args) -> std::pair<Item_T&, bool>
{
    if (const OptionalRef<Item_T> found{ find<Item_T>() }; found.has_value()) {
        return std::pair<Item_T&, bool>{ *found, false };
    }

    m_type_hashes.push_back(meta::hash<Item_T>());
    const ScopeFail destroy_type_hash_guard{
        [this] noexcept -> void { m_type_hashes.pop_back(); },
    };

    Any& erased_item =
        m_items.emplace_back(std::in_place_type<Item_T>, std::forward<Args_T>(args)...);

    return std::pair<Item_T&, bool>{
        any_cast<Item_T>(erased_item),
        true,
    };
}

}   // namespace ddge::util
