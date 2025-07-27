module;

#include <utility>

export module utility.meta.algorithms.for_each;

import utility.meta.algorithms.apply;
import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <index_sequence_c IndexSequence_T, typename F>
constexpr auto for_each(F&& func) -> F
{
    return apply<IndexSequence_T>(
        [func = std::forward<F>(func)]<size_t... indices_T> mutable -> F {
            (func.template operator()<indices_T>(), ...);
            return std::forward<F>(func);
        }
    );
}

export template <type_list_c TypeList_T, typename F>
constexpr auto for_each(F&& func) -> F
{
    for_each<std::make_index_sequence<type_list_size_v<TypeList_T>>>(
        [&func]<size_t index_T> {
            func.template operator()<type_list_at_t<TypeList_T, index_T>>();
        }
    );
    return std::forward<F>(func);
}

}   // namespace util::meta
