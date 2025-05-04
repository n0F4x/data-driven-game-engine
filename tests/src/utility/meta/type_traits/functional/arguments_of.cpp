#include <tuple>
#include <type_traits>

import utility.meta.type_traits.functional.arguments_of;

namespace {

constexpr auto lambda = [](const int) -> void {};

}   // namespace

static_assert(std::is_same_v<util::meta::arguments_of_t<decltype(lambda)>, std::tuple<int>>);
