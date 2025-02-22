module;

#include <utility>

export module utility.all_same;

import utility.meta.type_traits.all_same;

namespace util {

template <typename T, typename... Ts>
[[nodiscard]]
constexpr auto all_same_helper(T&& value, Ts&&... values) -> bool
{
    return ((value == values) && ...);
}

export template <typename... Ts>
[[nodiscard]]
constexpr auto all_same(Ts&&... values) -> bool
{
    if constexpr (sizeof...(Ts) == 0) {
        return true;
    }
    else {
        return all_same_helper(std::forward<Ts>(values)...);
    }
}

}   // namespace util
