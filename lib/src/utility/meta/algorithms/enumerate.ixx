module;

#include <utility>

export module utility.meta.algorithms.enumerate;

import utility.meta.algorithms.apply;
import utility.meta.concepts.type_list.type_list;

namespace util::meta {

export template <type_list_c TypeList_T>
constexpr inline auto enumerate = []<typename F>(F&& func) static -> F {
    return apply<TypeList_T>([&func]<typename... Ts> -> F {
        [&func]<size_t... indices_T>(std::index_sequence<indices_T...>) {
            (func.template operator()<indices_T, Ts...[indices_T]>(), ...);
        }(std::make_index_sequence<sizeof...(Ts)>{});

        return std::forward<F>(func);
    });
};

}   // namespace util::meta
