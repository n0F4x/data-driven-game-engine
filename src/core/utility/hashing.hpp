#pragma once

#include <ranges>

// Based on https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf

#include <functional>

namespace core {

template <template <typename> typename Hasher, typename T>
auto hash_combine(std::size_t& seed, const T& v) -> void
{
    Hasher<T> hasher;
    seed ^= hasher(v) + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
}

template <template <typename> typename Hasher, typename T, typename... Types>
auto hash_combine(std::size_t& seed, const T& val, const Types&... args) -> void
{
    hash_combine<Hasher>(seed, val);
    hash_combine<Hasher>(seed, args...);
}

template <template <typename> typename Hasher, typename... Types>
[[nodiscard]]
auto hash_combine(const Types&... args) -> size_t
{
    std::size_t seed = 0;
    hash_combine<Hasher>(seed, args...);
    return seed;
}

template <typename... Types>
[[nodiscard]]
auto hash_combine(const Types&... args) -> size_t
{
    std::size_t seed = 0;
    hash_combine<std::hash>(seed, args...);
    return seed;
}

template <template <typename> typename Hasher>
[[nodiscard]]
auto hash_range(std::ranges::sized_range auto&& t_range) -> size_t
{
    Hasher<std::ranges::range_value_t<decltype(t_range)>> hasher;
    std::size_t seed = std::ranges::size(t_range);
    for (auto x : t_range) {
        auto hash{ hasher(x) };
        hash = (hash >> 16 ^ hash) * 0x4'5d'9f'3b;
        hash = (hash >> 16 ^ hash) * 0x4'5d'9f'3b;
        hash = hash >> 16 ^ hash;
        seed ^= hash + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

[[nodiscard]]
auto hash_range(std::ranges::sized_range auto&& t_range) -> size_t
{
    return hash_range<std::hash>(std::forward<decltype(t_range)>(t_range));
}

}   // namespace core
