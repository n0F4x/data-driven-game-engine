module;

#include <utility>

export module ddge.utility.meta.algorithms.enumerate;

import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.type_list.type_list;
import ddge.utility.meta.type_traits.type_list.type_list_at;
import ddge.utility.meta.type_traits.type_list.type_list_size;

namespace ddge::util::meta {

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

}   // namespace ddge::util::meta
