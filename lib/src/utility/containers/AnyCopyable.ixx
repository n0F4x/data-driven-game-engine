module;

#include <cstddef>

export module ddge.utility.containers.AnyCopyable;

import ddge.utility.containers.Any;
import ddge.utility.memory.DefaultAllocator;
import ddge.utility.meta.concepts.generic_allocator;
import ddge.utility.meta.type_traits.always_true;

namespace ddge::util {

export template <
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = DefaultAllocator>
using BasicAnyCopyable = BasicAny<{}, meta::always_true, size_T, alignment_T, Allocator_T>;

export using AnyCopyable = BasicAnyCopyable<>;

}   // namespace ddge::util
