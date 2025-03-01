module;

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

export module utility.containers.MultiSparseSet;

import utility.containers.sparse_set.key_c;
import utility.containers.sparse_set.SparseSetBase;
import utility.containers.sparse_set.value_c;

import utility.meta.type_traits.forward_like;
import utility.ScopeGuard;

namespace util {

export template <
    typename Key_T,
    typename TypeList_T,
    uint8_t version_bits_T = sizeof(Key_T) * 2>
class MultiSparseSet;

export template <
    sparse_set::key_c Key_T,
    template <typename...> typename TypeList_T,
    sparse_set::value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
class MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>
    : sparse_set::SparseSetBase<Key_T, version_bits_T> {
    using Base = sparse_set::SparseSetBase<Key_T, version_bits_T>;

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
    using Index   = typename Base::Index;
    using Pointer = typename Base::Pointer;
    using Version = typename Base::Version;

    using Base::invalid_index;

    using Base::id_from_key;
    using Base::make_key;
    using Base::version_from_key;

    using Base::index_from_pointer;
    using Base::make_pointer;
    using Base::version_from_pointer;

    std::vector<Pointer>           m_pointers;
    std::tuple<std::vector<Ts>...> m_value_containers;
    std::vector<Index>             m_ids;
    Index                          oldest_dead_id{ invalid_index };
    Index                          youngest_dead_id{ invalid_index };
};

}   // namespace util

template <
    util::sparse_set::key_c Key_T,
    template <typename...> class TypeList_T,
    util::sparse_set::value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
template <typename... Us>
    requires(std::is_constructible_v<Ts, Us> && ...)
constexpr auto util::MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>::emplace(
    Us&&... values
) -> Key
{
    auto value_guards = [this, &values...]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple([this, &values...]<size_t I> {
            std::get<I>(m_value_containers).push_back(std::forward<Us...[I]>(values...[I]));
            return util::make_scope_guard([this] noexcept {
                std::get<I>(m_value_containers).pop_back();
            });
        }.template operator()<Is>()...);
    }(std::make_index_sequence<sizeof...(Us)>{});

    const Index index{ static_cast<Index>(std::get<0>(m_value_containers).size() - 1) };
    m_ids.emplace_back(index);
    ScopeGuard _{ util::make_scope_guard([this] noexcept { m_ids.pop_back(); }) };

    if (oldest_dead_id == invalid_index) {
        const Index   new_id{ static_cast<Index>(m_pointers.size()) };
        const Version new_version{};
        m_pointers.push_back(make_pointer(index, new_version));
        return make_key(new_id, new_version);
    }

    const Pointer old_pointer{ m_pointers[oldest_dead_id] };
    const Index   new_id{ oldest_dead_id };
    const Version new_version{ version_from_pointer(old_pointer) };
    oldest_dead_id = index_from_pointer(old_pointer);
    if (youngest_dead_id == new_id) {
        youngest_dead_id = invalid_index;
    }
    m_pointers[new_id] = make_pointer(index, new_version);
    return make_key(new_id, new_version);
}

template <
    util::sparse_set::key_c Key_T,
    template <typename...> class TypeList_T,
    util::sparse_set::value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>::erase(
    const Key key
) -> std::optional<std::tuple<Ts...>>
{
    const Index id{ id_from_key(key) };
    if (id >= m_pointers.size()) {
        return std::nullopt;
    }

    const Pointer pointer{ m_pointers[id] };
    const Index   index{ index_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (index == invalid_index || version_from_key(key) != version) {
        return std::nullopt;
    }

    std::tuple values = [this, index]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple([this, index]<size_t I> {
            auto value{ std::move(std::get<I>(m_value_containers)[index]) };

            std::get<I>(m_value_containers
            )[index] = std::move(std::get<I>(m_value_containers).back());
            std::get<I>(m_value_containers).pop_back();

            return value;
        }.template operator()<Is>()...);
    }(std::make_index_sequence<sizeof...(Ts)>{});

    const Index moved_id = m_ids[index] = std::move(m_ids.back());
    m_ids.pop_back();
    m_pointers[moved_id] = pointer;

    if (oldest_dead_id == invalid_index) {
        oldest_dead_id = id;
    }
    if (youngest_dead_id != invalid_index) {
        m_pointers[youngest_dead_id] =
            make_pointer(id, version_from_pointer(m_pointers[youngest_dead_id]));
    }
    youngest_dead_id = id;
    m_pointers[id]   = make_pointer(invalid_index, static_cast<Version>(version + 1));

    return values;
}

template <
    util::sparse_set::key_c Key_T,
    template <typename...> class TypeList_T,
    util::sparse_set::value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
template <typename Self_T>
constexpr auto util::MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>::get(
    this Self_T&& self,
    const Key     key
) -> std::tuple<meta::forward_like_t<Ts, Self_T>...>
{
    const Index id{ id_from_key(key) };
    const Key   pointer{ self.m_pointers[id] };

    assert(id < self.m_pointers.size());
    assert(version_from_key(key) == version_from_pointer(pointer));

    return [&self, pointer]<size_t... Is>(std::index_sequence<Is...>) {
        return std::forward_as_tuple([&self, pointer]<size_t I> -> decltype(auto) {
            return std::forward_like<Self_T>(std::get<I>(self.m_value_containers
            )[index_from_pointer(pointer)]);
        }.template operator()<Is>()...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <
    util::sparse_set::key_c Key_T,
    template <typename...> class TypeList_T,
    util::sparse_set::value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>::find(
    const Key key
) -> std::optional<std::tuple<Ts&...>>
{
    const Index id{ id_from_key(key) };
    if (id >= m_pointers.size()) {
        return std::nullopt;
    }

    const Pointer pointer{ m_pointers[id] };
    const Index   index{ index_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (version_from_key(key) != version) {
        return std::nullopt;
    }

    return [this, index]<size_t... Is>(std::index_sequence<Is...>) {
        return std::forward_as_tuple([this, index]<size_t I> -> decltype(auto) {
            return std::get<I>(m_value_containers)[index];
        }.template operator()<Is>()...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <
    util::sparse_set::key_c Key_T,
    template <typename...> class TypeList_T,
    util::sparse_set::value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>::find(
    const Key key
) const -> std::optional<std::tuple<const Ts&...>>
{
    return const_cast<MultiSparseSet&>(*this).find(key);
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

static_assert(
    [] {
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                              result = multi_sparse_set.get(key);

        static_assert(std::is_same_v<decltype(result), std::tuple<int&, float&, Dummy&>>);
        assert(result == values);

        return true;
    }(),
    "get & test failed"
);

static_assert(
    [] {
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto result = std::as_const(multi_sparse_set).get(key);

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
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                              result = std::move(multi_sparse_set).get(key);

        static_assert(std::is_same_v<decltype(result), std::tuple<int&&, float&&, Dummy&&>>);
        assert(result == values);

        return true;
    }(),
    "get && test failed"
);

static_assert(
    [] {
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key{ multi_sparse_set.emplace(
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
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key = multi_sparse_set.emplace(
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
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key = multi_sparse_set.emplace(
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
        util::MultiSparseSet<Key, Values> multi_sparse_set;

        assert(!multi_sparse_set.find(missing_key).has_value());

        return true;
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::MultiSparseSet<Key, Values> multi_sparse_set;
        const Key                         key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        const std::optional<Values>       erased_values = multi_sparse_set.erase(key);

        assert(erased_values.value() == values);
        assert(!multi_sparse_set.find(key).has_value());

        return true;
    }(),
    "erase contained value test failed"
);

#endif
