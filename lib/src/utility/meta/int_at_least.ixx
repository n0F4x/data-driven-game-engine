module;

#include <cstddef>
#include <cstdint>

export module utility.meta.int_at_least;

namespace util::meta {

export template <std::size_t>
struct int_at_least;

template <std::size_t SizeInBits_V>
    requires(SizeInBits_V <= 8)
struct int_at_least<SizeInBits_V> {
    using type = std::int_least8_t;
};

template <std::size_t SizeInBits_V>
    requires(SizeInBits_V <= 16 && SizeInBits_V > 8)
struct int_at_least<SizeInBits_V> {
    using type = std::int_least16_t;
};

template <std::size_t SizeInBits_V>
    requires(SizeInBits_V <= 32 && SizeInBits_V > 16)
struct int_at_least<SizeInBits_V> {
    using type = std::int_least32_t;
};

template <std::size_t SizeInBits_V>
    requires(SizeInBits_V > 32)
struct int_at_least<SizeInBits_V> {
    using type = std::int_least64_t;
};

export template <std::size_t SizeInBits_V>
using int_at_least_t = int_at_least<SizeInBits_V>;

}   // namespace util::meta
