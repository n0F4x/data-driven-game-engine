import ddge.utility.memory.DefaultAllocator;

import ddge.utility.meta.concepts.generic_allocator;

static_assert(ddge::util::meta::generic_allocator_c<ddge::util::DefaultAllocator>);
