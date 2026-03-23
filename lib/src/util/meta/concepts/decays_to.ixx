module;

#include <concepts>
#include <type_traits>

export module ddge.util.meta.concepts.decays_to;

namespace ddge::util::meta {

export template <typename T, typename U>
concept decays_to_c = std::same_as<std::decay_t<T>, U>;

}   // namespace ddge::util::meta
