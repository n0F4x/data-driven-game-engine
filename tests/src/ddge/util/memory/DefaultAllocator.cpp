import ddge.util.memory.DefaultAllocator;

import ddge.util.meta.concepts.generic_allocator;

static_assert(ddge::util::meta::generic_allocator_c<ddge::util::DefaultAllocator>);
