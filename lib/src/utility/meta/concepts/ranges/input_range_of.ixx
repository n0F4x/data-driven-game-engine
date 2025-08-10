module;

#include <concepts>
#include <ranges>

export module ddge.utility.meta.concepts.ranges.input_range_of;

namespace ddge::util::meta {

export template <typename Range_T, typename T>
concept input_range_of_c = std::ranges::input_range<Range_T>
                        && std::convertible_to<std::ranges::range_reference_t<Range_T>, T>;

}   // namespace ddge::util::meta
