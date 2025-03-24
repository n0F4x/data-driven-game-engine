module;

#include <functional>

export module utility.tuple.tuple_for_each;

import utility.meta.concepts.tuple_like;

namespace util {

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

}   // namespace util
