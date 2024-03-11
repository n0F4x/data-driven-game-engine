#pragma once

// Based on https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf

#include <functional>

namespace engine {

template <template <typename> typename Hasher, typename T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    Hasher<T> hasher;
    seed ^= hasher(v) + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
}

template <template <typename> typename Hasher, typename T, typename... Types>
void hash_combine(std::size_t& seed, const T& val, const Types&... args)
{
    hash_combine<Hasher>(seed, val);
    hash_combine<Hasher>(seed, args...);
}

template <template <typename> typename Hasher, typename... Types>
std::size_t hash_combine(const Types&... args)
{
    std::size_t seed = 0;
    hash_combine<Hasher>(seed, args...);
    return seed;
}

template <typename... Types>
std::size_t hash_combine(const Types&... args)
{
    std::size_t seed = 0;
    hash_combine<std::hash>(seed, args...);
    return seed;
}

}   // namespace engine
