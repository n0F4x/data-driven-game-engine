module;

#include <algorithm>
#include <array>
#include <ranges>
#include <utility>

export module ddge.util.meta.type_traits.integer_sequence.index_sequence_filter;

import ddge.util.meta.algorithms.apply;
import ddge.util.meta.concepts.integer_sequence.index_sequence;
import ddge.util.meta.type_traits.integer_sequence.integer_sequence_to;

namespace ddge::util::meta {

template <typename IndexSequence_T, template <std::size_t> typename Predicate_T>
struct index_sequence_filter_impl;

template <
    template <typename T_, T_...> typename IndexSequence_T,
    template <std::size_t> typename Predicate_T>
struct index_sequence_filter_impl<IndexSequence_T<std::size_t>, Predicate_T> {
    using type = IndexSequence_T<std::size_t>;
};

template <
    template <typename T_, T_...> typename IndexSequence_T,
    std::size_t... indices_T,
    template <std::size_t> typename Predicate_T>
struct index_sequence_filter_impl<IndexSequence_T<std::size_t, indices_T...>, Predicate_T> {
    constexpr static std::array<std::size_t, sizeof...(indices_T)> indices{ indices_T... };
    constexpr static std::array<std::size_t, sizeof...(indices_T)> mask{
        Predicate_T<indices_T>::value...
    };
    constexpr static std::size_t size{
        static_cast<std::size_t>(std::ranges::count(mask, true))
    };

    [[nodiscard]]
    constexpr static auto make_filtered_indices() -> std::array<std::size_t, size>
    {
        std::array<std::size_t, size> result;
        std::size_t                   valid_index{};
        for (std::size_t index{}; index < indices.size(); ++index) {
            if (mask[index]) {
                result[valid_index] = indices[index];
                ++valid_index;
            }
        }
        return result;
    }

    constexpr static auto helper()
    {
        constexpr static std::array filtered_indices{
            make_filtered_indices(),
        };

        return apply<std::make_index_sequence<filtered_indices.size()>>(
            [&]<std::size_t... x_indices_T>() {
                return std::index_sequence<filtered_indices[x_indices_T]...>{};
            }
        );
    }

    using type = integer_sequence_to_t<decltype(helper()), IndexSequence_T>;
};

export template <index_sequence_c IndexSequence_T, template <std::size_t> typename Predicate_T>
using index_sequence_filter_t =
    typename index_sequence_filter_impl<IndexSequence_T, Predicate_T>::type;

}   // namespace ddge::util::meta
