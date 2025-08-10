module;

#include <functional>

export module ddge.utility.tuple.tuple_for_each;

import ddge.utility.meta.concepts.tuple_like;

namespace ddge::util {

export template <meta::tuple_like_c Tuple_T, typename F>
constexpr auto tuple_for_each(Tuple_T&& tuple, F func) -> void
{
    std::apply(
        [&func]<typename... Ts>(Ts&&... values) {
            (std::invoke(func, std::forward<Ts>(values)), ...);
        },
        std::forward<Tuple_T>(tuple)
    );
}

}   // namespace ddge::util
