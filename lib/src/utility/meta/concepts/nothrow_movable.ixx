module;

#include <concepts>

export module ddge.utility.meta.concepts.nothrow_movable;

import ddge.utility.meta.concepts.nothrow_assignable_from;
import ddge.utility.meta.concepts.nothrow_move_constructible;

namespace ddge::util::meta {

export template <typename T>
concept nothrow_movable_c = std::is_object_v<T> && nothrow_move_constructible_c<T>
                         && nothrow_assignable_from_c<T&, T> && std::swappable<T>;

}   // namespace ddge::util::meta
