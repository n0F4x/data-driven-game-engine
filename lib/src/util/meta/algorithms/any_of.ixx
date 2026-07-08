module;

#include <utility>

export module ddge.util.meta.algorithms.any_of;

import ddge.util.meta.algorithms.apply;
import ddge.util.meta.concepts.integer_sequence.index_sequence;
import ddge.util.meta.concepts.type_list.type_list;
import ddge.util.meta.type_traits.type_list.type_list_at;
import ddge.util.meta.type_traits.type_list.type_list_size;

namespace ddge::util::meta {

export template <index_sequence_c IndexSequence_T, typename Predicate_T>
[[nodiscard]]
constexpr auto any_of(Predicate_T&& predicate) -> bool
{
    return apply<IndexSequence_T>([&predicate]<std::size_t... indices_T> -> bool {
        return (
            std::forward<Predicate_T>(predicate).template operator()<indices_T>() || ...
        );
    });
}

export template <type_list_c TypeList_T, typename Predicate_T>
[[nodiscard]]
constexpr auto any_of(Predicate_T&& predicate) -> bool
{
    return apply<TypeList_T>([&predicate]<typename... Ts> -> bool {
        return (std::forward<Predicate_T>(predicate).template operator()<Ts>() || ...);
    });
}

}   // namespace ddge::util::meta
