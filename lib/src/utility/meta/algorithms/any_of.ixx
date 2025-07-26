module;

#include <utility>

export module utility.meta.algorithms.any_of;

import utility.meta.algorithms.apply;
import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <typename T>
    requires index_sequence_c<T> || type_list_c<T>
struct AnyOfClosure;

export template <index_sequence_c IndexSequence_T>
struct AnyOfClosure<IndexSequence_T> {
    template <typename Predicate_T>
    [[nodiscard]]
    constexpr static auto operator()(Predicate_T predicate) -> bool
    {
        return apply<IndexSequence_T>([&predicate]<size_t... indices_T> -> bool {
            return (predicate.template operator()<indices_T>() || ...);
        });
    }
};

export template <type_list_c TypeList_T>
struct AnyOfClosure<TypeList_T> {
    template <typename Predicate_T>
    [[nodiscard]]
    constexpr static auto operator()(Predicate_T predicate) -> bool
    {
        return AnyOfClosure<std::make_index_sequence<type_list_size_v<TypeList_T>>>::
            operator()([&predicate]<size_t index_T> -> bool {
                return predicate.template operator()<type_list_at_t<TypeList_T, index_T>>(
                );
            });
    }
};

export template <typename T>
inline constexpr AnyOfClosure<T> any_of;

}   // namespace util::meta
