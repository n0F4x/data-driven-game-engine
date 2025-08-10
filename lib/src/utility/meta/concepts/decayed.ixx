module;

#include <concepts>

export module ddge.utility.meta.concepts.decayed;

namespace ddge::util::meta {

export template <typename T>
concept decayed_c = std::same_as<std::decay_t<T>, T>;

}   // namespace ddge::util::meta
