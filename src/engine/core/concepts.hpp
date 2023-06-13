#pragma once

#include <concepts>
#include <iterator>
#include <ranges>

namespace engine::utils {

template <typename Function, typename Result, typename... Args>
concept Invocable_R =
    std::regular_invocable<Function, Args...>
    && std::same_as<std::invoke_result_t<Function, Args...>, Result>;

template <class RangeType, typename UnderlyingType>
concept RangeOfConcept =
    std::ranges::range<RangeType>
    && std::convertible_to<
        std::iter_value_t<std::ranges::iterator_t<RangeType>>,
        UnderlyingType>;

}   // namespace engine::utils
