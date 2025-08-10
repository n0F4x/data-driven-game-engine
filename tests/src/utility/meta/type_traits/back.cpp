#include <type_traits>

import ddge.utility.meta.type_traits.back;

static_assert(std::is_same_v<ddge::util::meta::back_t<int>, int>);
static_assert(std::is_same_v<ddge::util::meta::back_t<int, long>, long>);
