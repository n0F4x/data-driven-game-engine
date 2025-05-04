module;

#include <array>
#include <type_traits>
#include <vector>

export module utility.meta.type_traits.integer_sequence.index_sequence_filter;

import utility.meta.algorithms.fold_left_first;
import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.type_traits.integer_sequence.integer_sequence_concat;

template <typename IndexSequence_T, template <size_t> typename Predicate_T>
struct index_sequence_filter_impl;

template <
    template <typename T_, T_...> typename IndexSequence_T,
    template <size_t> typename Predicate_T>
struct index_sequence_filter_impl<IndexSequence_T<size_t>, Predicate_T> {
    using type = IndexSequence_T<size_t>;
};

template <
    template <typename T_, T_...> typename IndexSequence_T,
    size_t... indices_T,
    template <size_t> typename Predicate_T>
struct index_sequence_filter_impl<IndexSequence_T<size_t, indices_T...>, Predicate_T> {
    using type = typename decltype(util::meta::fold_left_first<
                                   IndexSequence_T<size_t, indices_T...>>(
        []<size_t index_T> {
            return std::conditional_t<
                Predicate_T<index_T>::value,
                std::type_identity<IndexSequence_T<size_t, index_T>>,
                std::type_identity<IndexSequence_T<size_t>>>{};
        },
        []<typename Left_T,
           typename Right_T>(std::type_identity<Left_T>, std::type_identity<Right_T>) {
            return std::
                type_identity<util::meta::integer_sequence_concat_t<Left_T, Right_T>>{};
        }
    ))::type;
};

namespace util::meta {

template <index_sequence_c IndexSequence_T, template <size_t> typename Predicate_T>
struct index_sequence_filter {
    using type = typename index_sequence_filter_impl<IndexSequence_T, Predicate_T>::type;
};

export template <index_sequence_c IndexSequence_T, template <size_t> typename Predicate_T>
using index_sequence_filter_t =
    typename index_sequence_filter<IndexSequence_T, Predicate_T>::type;

}   // namespace util::meta
