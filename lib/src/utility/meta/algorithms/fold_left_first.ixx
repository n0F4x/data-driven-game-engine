module;

#include <functional>

export module utility.meta.algorithms.fold_left_first;

import utility.meta.algorithms.apply;
import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.integer_sequence.integer_sequence_size;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <typename T>
    requires index_sequence_c<T> || type_list_c<T>
struct FoldLeftFirstClosure;

export template <index_sequence_c IndexSequence_T>
    requires(integer_sequence_size_v<IndexSequence_T> != 0)
struct FoldLeftFirstClosure<IndexSequence_T> {
    template <typename Transform_T, typename Operation_T>
    constexpr static auto operator()(Transform_T transform, Operation_T operation)
    {
        return apply<IndexSequence_T>([&transform, &operation]<size_t... indices_T>() {
            return [&transform, &operation]<size_t index_T, typename Accumulated_T>(
                       this auto&& self, Accumulated_T&& accumulated
                   ) {
                if constexpr (index_T == sizeof...(indices_T)) {
                    return std::forward<Accumulated_T>(accumulated);
                }
                else {
                    return self.template operator()<index_T + 1>(std::invoke(
                        operation,
                        std::forward<Accumulated_T>(accumulated),
                        transform.template operator()<index_T>()
                    ));
                }
            }.template operator()<1>(transform.template operator()<indices_T...[0]>());
        });
    }
};

export template <type_list_c TypeList_T>
    requires(type_list_size_v<TypeList_T> != 0)
struct FoldLeftFirstClosure<TypeList_T> {
    template <typename Transform_T, typename Operation_T>
    constexpr static auto operator()(Transform_T transform, Operation_T operation)
    {
        return FoldLeftFirstClosure<std::make_index_sequence<type_list_size_v<TypeList_T>>>::
            operator()(
                [&transform]<size_t index_T> {
                    return transform.template operator(
                    )<type_list_at_t<TypeList_T, index_T>>();
                },
                operation
            );
    }
};

export template <typename T>
inline constexpr FoldLeftFirstClosure<T> fold_left_first;

}   // namespace util::meta
