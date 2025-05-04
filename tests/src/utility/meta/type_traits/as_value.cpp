#include <type_traits>

import utility.meta.type_traits.as_value;

static_assert(std::is_same_v<util::meta::as_value_t<int>, int>);

static_assert(std::is_same_v<util::meta::as_value_t<int&>, std::reference_wrapper<int>>);
static_assert(std::is_same_v<
              util::meta::as_value_t<const int&>,
              std::reference_wrapper<const int>>);

static_assert([]<typename T> {
    return !requires { std::type_identity<util::meta::as_value_t<T>>{}; };
}.operator()<int&&>());
static_assert([]<typename T> {
    return !requires { std::type_identity<util::meta::as_value_t<T>>{}; };
}.operator()<const int&&>());
