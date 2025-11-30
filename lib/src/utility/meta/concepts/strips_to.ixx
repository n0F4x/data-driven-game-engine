module;

#include <concepts>
#include <type_traits>

export module ddge.utility.meta.concepts.strips_to;

namespace ddge::util::meta {

export template <typename T, typename U>
concept strips_to_c = std::same_as<std::remove_cvref_t<T>, U>;

}   // namespace ddge::util::meta
