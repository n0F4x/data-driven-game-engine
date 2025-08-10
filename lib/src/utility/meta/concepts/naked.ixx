module;

#include <concepts>
#include <type_traits>

export module ddge.utility.meta.concepts.naked;

namespace ddge::util::meta {

export template <typename T>
concept naked_c = std::same_as<T, std::remove_cvref_t<T>>;

}   // namespace ddge::util::meta
