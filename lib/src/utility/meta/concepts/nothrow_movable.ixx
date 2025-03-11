module;

#include <concepts>

export module utility.meta.concepts.nothrow_movable;

import utility.meta.concepts.nothrow_assignable_from;
import utility.meta.concepts.nothrow_move_constructible;

namespace util::meta {

export template <typename T>
concept nothrow_movable_c = std::is_object_v<T> && nothrow_move_constructible_c<T>
                         && nothrow_assignable_from_c<T&, T> && std::swappable<T>;

}   // namespace util::meta
