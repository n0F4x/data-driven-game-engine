module;

#include <cstddef>

export module ddge.utility.containers.MoveOnlyFunction;

import ddge.utility.containers.Function;
import ddge.utility.meta.concepts.functional.function;

namespace ddge::util {

export using util::storable_in_function_c;

export template <
    meta::function_c Signature_T,
    std::size_t      size_T      = default_function_size(),
    std::size_t      alignment_T = default_function_alignment()>
using MoveOnlyFunction = Function<Signature_T, true, size_T, alignment_T>;

export using util::any_cast;

}   // namespace ddge::util
