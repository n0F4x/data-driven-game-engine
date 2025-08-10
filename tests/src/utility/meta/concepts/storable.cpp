#include <cstddef>

import ddge.utility.meta.concepts.storable;

struct S {};

struct S2 {
    ~S2() {}
};

struct S3 {
    ~S3() noexcept(false) {}
};

static_assert(!ddge::util::meta::storable_c<void>);
static_assert(ddge::util::meta::storable_c<std::nullptr_t>);
static_assert(ddge::util::meta::storable_c<int>);
static_assert(ddge::util::meta::storable_c<int[10]>);
static_assert(!ddge::util::meta::storable_c<int[]>);
static_assert(ddge::util::meta::storable_c<S>);
static_assert(ddge::util::meta::storable_c<void*>);
static_assert(ddge::util::meta::storable_c<const int>);
static_assert(ddge::util::meta::storable_c<const volatile int>);
static_assert(ddge::util::meta::storable_c<int&>);
static_assert(ddge::util::meta::storable_c<const int&>);
static_assert(ddge::util::meta::storable_c<const volatile int&>);
static_assert(ddge::util::meta::storable_c<int&&>);
static_assert(ddge::util::meta::storable_c<const int&&>);
static_assert(ddge::util::meta::storable_c<const volatile int&&>);
static_assert(ddge::util::meta::storable_c<int S::*>);
static_assert(ddge::util::meta::storable_c<int (S::*)(int)>);
static_assert(!ddge::util::meta::storable_c<void()>);
static_assert(ddge::util::meta::storable_c<void (&)()>);
static_assert(ddge::util::meta::storable_c<void (*)()>);
static_assert(ddge::util::meta::storable_c<S2>);
static_assert(!ddge::util::meta::storable_c<S3>);
