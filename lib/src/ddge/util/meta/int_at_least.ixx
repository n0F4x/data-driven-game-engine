module;

#include <cstddef>
#include <cstdint>

export module ddge.util.meta.int_at_least;

namespace ddge::util::meta {

export template <std::size_t>
struct int_at_least;

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T <= 8)
struct int_at_least<size_in_bits_T> {
    using type = std::int_least8_t;
};

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T <= 16 && size_in_bits_T > 8)
struct int_at_least<size_in_bits_T> {
    using type = std::int_least16_t;
};

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T <= 32 && size_in_bits_T > 16)
struct int_at_least<size_in_bits_T> {
    using type = std::int_least32_t;
};

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T > 32)
struct int_at_least<size_in_bits_T> {
    using type = std::int_least64_t;
};

export template <std::size_t size_in_bits_T>
using int_at_least_t = int_at_least<size_in_bits_T>;

}   // namespace ddge::util::meta
