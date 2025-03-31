module;

#include <utility>

export module utility.meta.algorithms.for_each;

import utility.meta.algorithms.apply;
import utility.meta.concepts.type_list.type_list;

namespace util::meta {

export template <meta::type_list_c TypeList_T>
constexpr inline auto for_each = []<typename F>(F&& func) -> F {
    return apply<TypeList_T>([&func]<typename... Ts> -> F {
        [&func]<size_t... indices_T>(std::index_sequence<indices_T...>) {
            (func.template operator()<Ts...[indices_T]>(), ...);
        }(std::make_index_sequence<sizeof...(Ts)>{});

        return std::forward<F>(func);
    });
};

}   // namespace util::meta
