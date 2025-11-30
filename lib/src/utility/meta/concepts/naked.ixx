module;

#include <type_traits>

export module ddge.utility.meta.concepts.naked;

import ddge.utility.meta.concepts.strips_to;

namespace ddge::util::meta {

export template <typename T>
concept naked_c = strips_to_c<T, std::remove_cvref_t<T>>;

}   // namespace ddge::util::meta
