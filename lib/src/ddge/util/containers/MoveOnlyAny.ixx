module;

#include <cstddef>

export module ddge.util.containers.MoveOnlyAny;

import ddge.util.containers.Any;

namespace ddge::util {

export using util::storable_in_any_c;

export using util::default_any_size;
export using util::default_any_alignment;
export using util::DefaultAnyPolicy;

export template <
    std::size_t size_T                    = default_any_size(),
    std::size_t alignment_T               = default_any_alignment(),
    template <typename> typename Policy_T = DefaultAnyPolicy>
using BasicMoveOnlyAny = BasicAny<true, size_T, alignment_T, Policy_T>;

export using MoveOnlyAny = BasicMoveOnlyAny<>;

export template <typename T>
concept move_only_any_c = any_c<T> && (T::is_move_only());

export using util::any_cast;

}   // namespace ddge::util
