export module utility.meta.algorithms.apply;

import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_to;
import utility.TypeList;

namespace util::meta {

export template <meta::type_list_c TypeList_T>
constexpr inline auto apply = []<typename F>(F func) static -> decltype(auto) {
    return [&func]<typename... Ts>(TypeList<Ts...>) -> decltype(auto) {
        return func.template operator()<Ts...>();
    }(meta::type_list_to_t<TypeList_T, TypeList>{});
};

}   // namespace util::meta
