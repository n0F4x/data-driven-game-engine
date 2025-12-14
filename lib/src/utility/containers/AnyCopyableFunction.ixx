module;

#include <cstddef>

export module ddge.utility.containers.AnyCopyableFunction;

import ddge.utility.containers.Any;
import ddge.utility.containers.AnyFunction;
import ddge.utility.memory.DefaultAllocator;
import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.generic_allocator;

namespace ddge::util {

export template <
    meta::function_c                      Signature_T,
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = DefaultAllocator>
using AnyCopyableFunction =
    AnyFunction<Signature_T, ddge::util::AnyProperties{}, size_T, alignment_T, Allocator_T>;

}   // namespace ddge::util
