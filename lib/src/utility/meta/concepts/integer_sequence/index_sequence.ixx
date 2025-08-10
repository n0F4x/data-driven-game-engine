export module ddge.utility.meta.concepts.integer_sequence.index_sequence;

import ddge.utility.meta.type_traits.integer_sequence.is_index_sequence;

namespace ddge::util::meta {

export template <typename T>
concept index_sequence_c = is_index_sequence_v<T>;

}   // namespace ddge::util::meta
