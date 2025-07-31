module;

#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

export module utility.containers.SlotMultiMap;

import utility.containers.SparseSet;

import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.ScopeGuard;
import utility.Strong;

template <typename T>
concept key_c = requires { util::SparseSet<T>{}; };

namespace util {

export template <
    ::key_c Key_T,
    typename TypeList_T,
    uint8_t version_bit_size_T = sizeof(Key_T) * 2>
class SlotMultiMap;

template <
    typename Key_T,
    template <typename...> typename TypeList_T,
    ::util::meta::nothrow_movable_c... Ts,
    uint8_t version_bit_size_T>
class SlotMultiMap<Key_T, TypeList_T<Ts...>, version_bit_size_T> {
public:
    using Key = Key_T;

    template <typename... Us>
        requires(std::is_constructible_v<Ts, Us> && ...)
    constexpr auto emplace(Us&&... values) -> Key;

    constexpr auto erase(Key key) -> std::optional<std::tuple<Ts...>>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&, Key key)
        -> std::tuple<meta::forward_like_t<Ts, Self_T>...>;

    [[nodiscard]]
    constexpr auto find(Key key) -> std::optional<std::tuple<Ts&...>>;
    [[nodiscard]]
    constexpr auto find(Key key) const -> std::optional<std::tuple<const Ts&...>>;

private:
    SparseSet<Key, version_bit_size_T> m_sparse_set;
    std::tuple<std::vector<Ts>...>     m_value_containers;
};

}   // namespace util

template <
    typename Key_T,
    template <typename...> typename TypeList_T,
    ::util::meta::nothrow_movable_c... Ts,
    uint8_t version_bit_size_T>
template <typename... Us>
    requires(std::is_constructible_v<Ts, Us> && ...)
constexpr auto util::SlotMultiMap<Key_T, TypeList_T<Ts...>, version_bit_size_T>::emplace(
    Us&&... values
) -> Key
{
    ScopeGuards _ = [this, &values...]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(ScopeGuard{ [this, &values...]<std::size_t I> {
            std::get<I>(m_value_containers).push_back(std::forward<Us...[I]>(values...[I]));
            return [this] noexcept { std::get<I>(m_value_containers).pop_back(); };
        }.template operator()<Is>() }...);
    }(std::make_index_sequence<sizeof...(Us)>{});

    return m_sparse_set.emplace().first;
}

template <
    typename Key_T,
    template <typename...> typename TypeList_T,
    ::util::meta::nothrow_movable_c... Ts,
    uint8_t version_bit_size_T>
constexpr auto util::SlotMultiMap<Key_T, TypeList_T<Ts...>, version_bit_size_T>::erase(
    const Key key
) -> std::optional<std::tuple<Ts...>>
{
    return m_sparse_set.erase(key).transform([this](const auto index) {
        return [this, index]<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::make_tuple([this, index]<std::size_t I> {
                auto value{ std::move(std::get<I>(m_value_containers)[index]) };

                std::get<I>(m_value_containers
                )[index] = std::move(std::get<I>(m_value_containers).back());
                std::get<I>(m_value_containers).pop_back();

                return value;
            }.template operator()<Is>()...);
        }(std::make_index_sequence<sizeof...(Ts)>{});
    });
}

template <
    typename Key_T,
    template <typename...> typename TypeList_T,
    ::util::meta::nothrow_movable_c... Ts,
    uint8_t version_bit_size_T>
template <typename Self_T>
constexpr auto util::SlotMultiMap<Key_T, TypeList_T<Ts...>, version_bit_size_T>::get(
    this Self_T&& self,
    const Key     key
) -> std::tuple<meta::forward_like_t<Ts, Self_T>...>
{
    const auto index = self.m_sparse_set.get(key);

    return [&self, index]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::forward_as_tuple([&self, index]<std::size_t I> -> decltype(auto) {
            return std::forward_like<Self_T>(std::get<I>(self.m_value_containers)[index]);
        }.template operator()<Is>()...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <
    typename Key_T,
    template <typename...> typename TypeList_T,
    ::util::meta::nothrow_movable_c... Ts,
    uint8_t version_bit_size_T>
constexpr auto util::SlotMultiMap<Key_T, TypeList_T<Ts...>, version_bit_size_T>::find(
    const Key key
) -> std::optional<std::tuple<Ts&...>>
{
    return m_sparse_set.find(key).transform([this](const auto index) {
        return [this, index]<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::forward_as_tuple([this, index]<std::size_t I> -> decltype(auto) {
                return std::get<I>(m_value_containers)[index];
            }.template operator()<Is>()...);
        }(std::make_index_sequence<sizeof...(Ts)>{});
    });
}

template <
    typename Key_T,
    template <typename...> typename TypeList_T,
    ::util::meta::nothrow_movable_c... Ts,
    uint8_t version_bit_size_T>
constexpr auto util::SlotMultiMap<Key_T, TypeList_T<Ts...>, version_bit_size_T>::find(
    const Key key
) const -> std::optional<std::tuple<const Ts&...>>
{
    return const_cast<SlotMultiMap&>(*this).find(key);
}
