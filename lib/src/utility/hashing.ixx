module;

#include <ranges>

// Based on https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf

#include <functional>

export module utility.hashing;

namespace util {

export template <template <typename> typename Hasher, typename T>
auto hash_combine(std::size_t& seed, const T& v) -> void
{
    Hasher<T> hasher;
    seed ^= hasher(v) + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
}

export template <template <typename> typename Hasher, typename T, typename... Types>
auto hash_combine(std::size_t& seed, const T& val, const Types&... args) -> void
{
    hash_combine<Hasher>(seed, val);
    hash_combine<Hasher>(seed, args...);
}

export template <template <typename> typename Hasher, typename... Types>
[[nodiscard]]
auto hash_combine(const Types&... args) -> std::size_t
{
    std::size_t seed = 0;
    hash_combine<Hasher>(seed, args...);
    return seed;
}

export template <typename... Types>
[[nodiscard]]
auto hash_combine(const Types&... args) -> std::size_t
{
    std::size_t seed = 0;
    hash_combine<std::hash>(seed, args...);
    return seed;
}

export template <template <typename> typename Hasher>
[[nodiscard]]
auto hash_range(std::ranges::sized_range auto&& range) -> std::size_t
{
    Hasher<std::ranges::range_value_t<decltype(range)>> hasher;
    std::size_t                                         seed = std::ranges::size(range);
    for (auto x : range) {
        auto hash{ hasher(x) };
        hash = (hash >> 16 ^ hash) * 0x4'5d'9f'3b;
        hash = (hash >> 16 ^ hash) * 0x4'5d'9f'3b;
        hash = hash >> 16 ^ hash;
        seed ^= hash + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

export [[nodiscard]]
auto hash_range(std::ranges::sized_range auto&& range) -> std::size_t
{
    return hash_range<std::hash>(std::forward<decltype(range)>(range));
}

}   // namespace util
