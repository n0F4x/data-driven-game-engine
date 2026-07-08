module;

#include <cstddef>
#include <cstdint>

export module ddge.util.meta.uint_at_least;

namespace ddge::util::meta {

export template <std::size_t>
struct uint_at_least;

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T <= 8)
struct uint_at_least<size_in_bits_T> {
    using type = std::uint_least8_t;
};

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T <= 16 && size_in_bits_T > 8)
struct uint_at_least<size_in_bits_T> {
    using type = std::uint_least16_t;
};

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T <= 32 && size_in_bits_T > 16)
struct uint_at_least<size_in_bits_T> {
    using type = std::uint_least32_t;
};

template <std::size_t size_in_bits_T>
    requires(size_in_bits_T > 32)
struct uint_at_least<size_in_bits_T> {
    using type = std::uint_least64_t;
};

export template <std::size_t size_in_bits_T>
using uint_at_least_t = typename uint_at_least<size_in_bits_T>::type;

}   // namespace ddge::util::meta
