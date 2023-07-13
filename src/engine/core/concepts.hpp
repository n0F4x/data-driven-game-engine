#pragma once

#include <concepts>
#include <iterator>
#include <ranges>

namespace engine::utils {

template <class RangeType, typename UnderlyingType>
concept RangeOfConcept =
    std::ranges::range<RangeType>
    && std::convertible_to<
        std::iter_value_t<std::ranges::iterator_t<RangeType>>,
        UnderlyingType>;

}   // namespace engine::utils
