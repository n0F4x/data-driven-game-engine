module;

#include <algorithm>
#include <array>
#include <iterator>
#include <utility>

export module utility.meta.type_traits.integer_sequence.integer_sequence_unique;

import utility.meta.algorithms.apply;
import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

template <integer_sequence_c IntegerSequence_T>
struct integer_sequence_unique;

template <
    template <typename T_, T_...> typename IntegerSequence,
    typename Integer_T,
    Integer_T... integers_T>
struct integer_sequence_unique<IntegerSequence<Integer_T, integers_T...>> {
    using type = decltype([] {
        // TODO: constexpr structured binding with p2686r5
        constexpr static std::pair unique_integers_and_count = [] {
            std::array<Integer_T, sizeof...(integers_T)> integers_array{ integers_T... };
            std::ranges::sort(integers_array);
            const auto unique_count = std::distance(
                integers_array.begin(),
                std::unique(integers_array.begin(), integers_array.end())
            );
            return std::make_pair(integers_array, unique_count);
        }();

        return util::meta::apply<std::make_index_sequence<
            unique_integers_and_count.second>>([&]<size_t... indices_T>() {
            return std::
                integer_sequence<Integer_T, unique_integers_and_count.first[indices_T]...>(
                );
        });
    }());
};

export template <integer_sequence_c IntegerSequence_T>
using integer_sequence_unique_t =
    typename integer_sequence_unique<IntegerSequence_T>::type;

}   // namespace util::meta
