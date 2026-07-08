module;

#include <concepts>
#include <type_traits>

export module ddge.util.meta.concepts.decayed;

namespace ddge::util::meta {

export template <typename T>
concept decayed_c = std::same_as<T, std::decay_t<T>>;

}   // namespace ddge::util::meta
