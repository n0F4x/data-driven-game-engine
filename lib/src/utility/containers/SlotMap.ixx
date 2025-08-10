module;

#include <cassert>
#include <functional>
#include <optional>
#include <span>
#include <utility>
#include <vector>

#include "utility/lifetime_bound.hpp"

export module ddge.utility.containers.SlotMap;

import ddge.utility.containers.SparseSet;

import ddge.utility.meta.concepts.nothrow_movable;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.containers.OptionalRef;
import ddge.utility.ScopeGuard;
import ddge.utility.Strong;

template <typename T>
concept key_c = requires { ddge::util::SparseSet<T>{}; };

namespace ddge::util {

export template <
    ::key_c                             Key_T,
    ddge::util::meta::nothrow_movable_c T,
    uint8_t                             version_bit_size_T = sizeof(Key_T) * 2>
class SlotMap {
    using SparseSet = SparseSet<Key_T, version_bit_size_T>;

public:
    using Key   = Key_T;
    using Index = typename SparseSet::ID;
    using Value = T;

    [[nodiscard]]
    constexpr auto next_key() const noexcept -> Key;

    template <typename... Args_T>
    constexpr auto emplace(Args_T&&... args) -> std::pair<Key, Index>;
    constexpr auto erase(Key key) -> std::optional<std::pair<Value, Index>>;
    constexpr auto remove(Key key) -> std::pair<Value, Index>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&, Key key) -> meta::forward_like_t<Value, Self_T>;
    [[nodiscard]]
    constexpr auto find(Key key) noexcept -> OptionalRef<Value>;
    [[nodiscard]]
    constexpr auto find(Key key) const noexcept -> OptionalRef<const Value>;
    [[nodiscard]]
    constexpr auto contains(Key key) const noexcept -> bool;
    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;
    [[nodiscard]]
    constexpr auto values() const noexcept [[lifetime_bound]] -> std::span<const Value>;

    [[nodiscard]]
    constexpr auto get_index(Key key) const -> Index;
    [[nodiscard]]
    constexpr auto find_index(Key key) const noexcept -> std::optional<Index>;

private:
    SparseSet      m_sparse_set;
    std::vector<T> m_values;
};

}   // namespace ddge::util

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::next_key() const noexcept
    -> Key
{
    return m_sparse_set.next_key();
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
template <typename... Args_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::emplace(Args_T&&... args)
    -> std::pair<Key, Index>
{
    m_values.emplace_back(std::forward<Args_T>(args)...);
    ScopeGuard _{ [this] noexcept { m_values.pop_back(); } };

    return m_sparse_set.emplace();
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::erase(const Key key)
    -> std::optional<std::pair<Value, Index>>
{
    return m_sparse_set.erase(key).transform([this](const auto index) {
        std::tuple result{ std::move(m_values[index]), index };

        m_values[index] = std::move(m_values.back());
        m_values.pop_back();

        return result;
    });
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::remove(const Key key)
    -> std::pair<Value, Index>
{
    const Index index = m_sparse_set.remove(key);

    std::tuple result{ std::move(m_values[index]), index };

    m_values[index] = std::move(m_values.back());
    m_values.pop_back();

    return result;
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
template <typename Self>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::get(
    this Self&& self,
    const Key   key
) -> meta::forward_like_t<Value, Self>
{
    const auto index{ self.m_sparse_set.get(key) };

    return std::forward_like<Self>(self.m_values[index]);
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::find(
    const Key key
) noexcept -> OptionalRef<Value>
{
    return m_sparse_set.find(key).transform([this](const auto index) {
        return std::ref(m_values[index]);
    });
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::find(
    const Key key
) const noexcept -> OptionalRef<const Value>
{
    return const_cast<SlotMap&>(*this).find(key);
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::contains(
    const Key key
) const noexcept -> bool
{
    return m_sparse_set.contains(key);
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::empty() const noexcept
    -> bool
{
    assert(m_sparse_set.empty() == m_values.empty());
    return m_sparse_set.empty();
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::values() const noexcept
    -> std::span<const Value>
{
    return m_values;
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::get_index(
    const Key key
) const -> Index
{
    return m_sparse_set.get(key);
}

template <::key_c Key_T, ddge::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto ddge::util::SlotMap<Key_T, T, version_bit_size_T>::find_index(
    const Key key
) const noexcept -> std::optional<Index>
{
    return m_sparse_set.find(key);
}
