module;

#include <type_traits>

export module ddge.utility.meta.concepts.contained_exactly_once;

namespace ddge::util::meta {

export template <typename T, typename... Ts>
concept contained_exactly_once_c = (int{ std::is_same_v<T, Ts> } + ...) == 1;

}   // namespace ddge::util
