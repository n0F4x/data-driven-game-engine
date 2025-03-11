module;

#include <cstdint>

export module utility.meta.uint_at_least;

namespace util::meta {

export template <size_t>
struct uint_at_least;

template <size_t SizeInBits_V>
    requires(SizeInBits_V <= 8)
struct uint_at_least<SizeInBits_V> {
    using type = std::uint_least8_t;
};

template <size_t SizeInBits_V>
    requires(SizeInBits_V <= 16 && SizeInBits_V > 8)
struct uint_at_least<SizeInBits_V> {
    using type = std::uint_least16_t;
};

template <size_t SizeInBits_V>
    requires(SizeInBits_V <= 32 && SizeInBits_V > 16)
struct uint_at_least<SizeInBits_V> {
    using type = std::uint_least32_t;
};

template <size_t SizeInBits_V>
    requires(SizeInBits_V > 32)
struct uint_at_least<SizeInBits_V> {
    using type = std::uint_least64_t;
};

export template <size_t SizeInBits_V>
using uint_at_least_t = typename uint_at_least<SizeInBits_V>::type;

}   // namespace util::meta
