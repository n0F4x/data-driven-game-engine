module;

#include <cstddef>

export module ddge.util.containers.CopyableAny;

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
using BasicCopyableAny = BasicAny<false, size_T, alignment_T, Policy_T>;

export using CopyableAny = BasicCopyableAny<>;

export template <typename T>
concept copyable_any_c = any_c<T> && (!T::is_move_only());

export using util::any_cast;

}   // namespace ddge::util
