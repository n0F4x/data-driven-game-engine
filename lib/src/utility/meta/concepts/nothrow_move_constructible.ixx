export module ddge.utility.meta.concepts.nothrow_move_constructible;

import ddge.utility.meta.concepts.nothrow_constructible_from;
import ddge.utility.meta.concepts.nothrow_convertible_to;

namespace ddge::util::meta {

export template <typename T>
concept nothrow_move_constructible_c = nothrow_constructible_from_c<T, T>
                                    && nothrow_convertible_to_c<T, T>;

}   // namespace ddge::util::meta
