module;

#include <utility>

export module ddge.util.meta.algorithms.for_each;

import ddge.util.meta.algorithms.apply;
import ddge.util.meta.concepts.integer_sequence.index_sequence;
import ddge.util.meta.concepts.type_list.type_list;
import ddge.util.meta.type_traits.type_list.type_list_at;
import ddge.util.meta.type_traits.type_list.type_list_size;

namespace ddge::util::meta {

export template <index_sequence_c IndexSequence_T, typename F>
constexpr auto for_each(F&& func) -> F
{
    return apply<IndexSequence_T>(
        [func = std::forward<F>(func)]<std::size_t... indices_T> mutable -> F {
            (func.template operator()<indices_T>(), ...);
            return std::forward<F>(func);
        }
    );
}

export template <type_list_c TypeList_T, typename F>
constexpr auto for_each(F&& func) -> F
{
    for_each<std::make_index_sequence<type_list_size_v<TypeList_T>>>(
        [&func]<std::size_t index_T> {
            func.template operator()<type_list_at_t<TypeList_T, index_T>>();
        }
    );
    return std::forward<F>(func);
}

}   // namespace ddge::util::meta
