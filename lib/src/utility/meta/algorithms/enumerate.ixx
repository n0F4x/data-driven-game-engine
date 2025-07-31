module;

#include <utility>

export module utility.meta.algorithms.enumerate;

import utility.meta.algorithms.for_each;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <type_list_c TypeList_T, typename F>
constexpr auto enumerate(F&& func) -> F
{
    for_each<std::make_index_sequence<type_list_size_v<TypeList_T>>>(
        [&func]<std::size_t index_T> {
            func.template operator()<index_T, type_list_at_t<TypeList_T, index_T>>();
        }
    );
    return std::forward<F>(func);
}

}   // namespace util::meta
