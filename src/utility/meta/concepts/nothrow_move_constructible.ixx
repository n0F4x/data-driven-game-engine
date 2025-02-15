export module utility.meta.concepts.nothrow_move_constructible;

import utility.meta.concepts.nothrow_constructible_from;
import utility.meta.concepts.nothrow_convertible_to;

namespace util::meta {

export template <typename T>
concept nothrow_move_constructible_c = nothrow_constructible_from_c<T, T>
                                    && nothrow_convertible_to_c<T, T>;

}   // namespace util::meta
