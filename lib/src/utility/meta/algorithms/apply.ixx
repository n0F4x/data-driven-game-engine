module;

#include <utility>

export module utility.meta.algorithms.apply;

import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.integer_sequence.integer_sequence_to;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;
import utility.ValueSequence;

namespace util::meta {

export template <typename T>
    requires index_sequence_c<T> || type_list_c<T>
struct ApplyClosure;

export template <index_sequence_c IndexSequence_T>
struct ApplyClosure<IndexSequence_T> {
    template <typename F>
    constexpr static auto operator()(F func) -> decltype(auto)
    {
        return [&func]<size_t... indices_T>(ValueSequence<size_t, indices_T...>)
                   -> decltype(auto) {
            return func.template operator()<indices_T...>();
        }(integer_sequence_to_t<IndexSequence_T, ValueSequence>{});
    }
};

export template <type_list_c TypeList_T>
struct ApplyClosure<TypeList_T> {
    template <typename F>
    constexpr static auto operator()(F func) -> decltype(auto)
    {
        return ApplyClosure<std::make_index_sequence<type_list_size_v<TypeList_T>>>::
            operator()([&func]<size_t... indices_T> -> decltype(auto) {
                return func.template operator()<type_list_at_t<TypeList_T, indices_T>...>(
                );
            });
    }
};

export template <typename T>
constexpr inline ApplyClosure<T> apply;

/// whole -> whole ✅
/// whole -> iterated ✅
/// iterated -> iterated ✅
/// iterated -> whole ❌ - we need a way to merge
///
/// apply<TypeList>(enumerate, any_of(pred))

}   // namespace util::meta
