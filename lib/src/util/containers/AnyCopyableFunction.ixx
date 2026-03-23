module;

#include <cstddef>

export module ddge.util.containers.AnyCopyableFunction;

import ddge.util.containers.Any;
import ddge.util.containers.AnyFunction;
import ddge.util.memory.DefaultAllocator;
import ddge.util.meta.concepts.functional.function;
import ddge.util.meta.concepts.generic_allocator;

namespace ddge::util {

export template <
    meta::function_c                      Signature_T,
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = DefaultAllocator>
using AnyCopyableFunction =
    AnyFunction<Signature_T, ddge::util::AnyProperties{}, size_T, alignment_T, Allocator_T>;

}   // namespace ddge::util
