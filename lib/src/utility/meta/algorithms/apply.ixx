module;

#include <utility>

export module utility.meta.algorithms.apply;

import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.integer_sequence.integer_sequence_to;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.type_list.type_list_to;
import utility.TypeList;
import utility.ValueSequence;

namespace util::meta {

export template <index_sequence_c IndexSequence_T, typename F>
constexpr auto apply(F&& func) -> decltype(auto)
{
    return
        [&func]<std::size_t... indices_T>(ValueSequence<std::size_t, indices_T...>) -> decltype(auto
                                                                          ) {
            return std::forward<F>(func).template operator()<indices_T...>();
        }(integer_sequence_to_t<IndexSequence_T, ValueSequence>{});
}

export template <type_list_c TypeList_T, typename F>
constexpr auto apply(F&& func) -> decltype(auto)
{
    return [&func]<typename... Ts>(TypeList<Ts...>) -> decltype(auto) {
        return std::forward<F>(func).template operator()<Ts...>();
    }(type_list_to_t<TypeList_T, TypeList>{});
}

}   // namespace util::meta
