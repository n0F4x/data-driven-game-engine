module;

#include <cstddef>

export module ddge.util.containers.CopyableFunction;

import ddge.util.containers.Function;
import ddge.util.meta.concepts.functional.function;

namespace ddge::util {

export using util::storable_in_function_c;

export template <
    meta::function_c Signature_T,
    std::size_t      size_T      = default_function_size(),
    std::size_t      alignment_T = default_function_alignment()>
using CopyableFunction = Function<Signature_T, false, size_T, alignment_T>;

export using util::any_cast;

}   // namespace ddge::util
