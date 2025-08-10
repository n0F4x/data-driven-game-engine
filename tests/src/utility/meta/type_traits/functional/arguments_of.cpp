#include <type_traits>

import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.TypeList;

namespace {

constexpr auto lambda = [](const int) -> void {};

}   // namespace

static_assert(std::is_same_v<
              ddge::util::meta::arguments_of_t<decltype(lambda)>,
              ddge::util::TypeList<int>>);
