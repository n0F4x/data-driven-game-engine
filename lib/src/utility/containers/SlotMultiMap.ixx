module;

#include <cassert>
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

// TODO: remove this namespace
namespace {
template <typename T>
concept key_c = requires { util::SparseSet<T>{}; };
}   // namespace

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
    ScopeGuards _ = [this, &values...]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(ScopeGuard{ [this, &values...]<size_t I> {
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
        return [this, index]<size_t... Is>(std::index_sequence<Is...>) {
            return std::make_tuple([this, index]<size_t I> {
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

    return [&self, index]<size_t... Is>(std::index_sequence<Is...>) {
        return std::forward_as_tuple([&self, index]<size_t I> -> decltype(auto) {
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
        return [this, index]<size_t... Is>(std::index_sequence<Is...>) {
            return std::forward_as_tuple([this, index]<size_t I> -> decltype(auto) {
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

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key = uint32_t;

struct Dummy {
    double value{};

    auto operator<=>(const Dummy&) const = default;
};

using Values = std::tuple<int, float, Dummy>;

constexpr Key                           missing_key{ std::numeric_limits<Key>::max() };
constexpr std::tuple<int, float, Dummy> values{ 32, 0.7f, Dummy{ 9 } };

template <>
class util::SlotMultiMap<Key, std::tuple<>>;

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                            result = multi_sparse_set.get(key);

        static_assert(std::is_same_v<decltype(result), std::tuple<int&, float&, Dummy&>>);
        assert(result == values);

        return true;
    }(),
    "get & test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                            result = std::as_const(multi_sparse_set).get(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::tuple<const int&, const float&, const Dummy&>>);
        assert(result == values);

        return true;
    }(),
    "get const& test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                            result = std::move(multi_sparse_set).get(key);

        static_assert(std::is_same_v<decltype(result), std::tuple<int&&, float&&, Dummy&&>>);
        assert(result == values);

        return true;
    }(),
    "get && test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto result = std::move(std::as_const(multi_sparse_set)).get(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::tuple<const int&&, const float&&, const Dummy&&>>);
        assert(result == values);

        return true;
    }(),
    "get const&& test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key = multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        );
        auto result = multi_sparse_set.find(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::optional<std::tuple<int&, float&, Dummy&>>>);
        assert(result.value() == values);

        return true;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key = multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        );
        auto result = std::as_const(multi_sparse_set).find(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::optional<std::tuple<const int&, const float&, const Dummy&>>>);
        assert(result.value() == values);

        return true;
    }(),
    "find const contained value test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;

        assert(!multi_sparse_set.find(missing_key).has_value());

        return true;
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        const std::optional<Values>     erased_values = multi_sparse_set.erase(key);

        assert(erased_values.value() == values);
        assert(!multi_sparse_set.find(key).has_value());

        return true;
    }(),
    "erase contained value test failed"
);

#endif
