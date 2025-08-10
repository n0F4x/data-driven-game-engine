export module ddge.utility.meta.concepts.integer_sequence.integer_sequence;

import ddge.utility.meta.type_traits.integer_sequence.is_integer_sequence;

namespace ddge::util::meta {

export template <typename T>
concept integer_sequence_c = is_integer_sequence_v<T>;

}   // namespace ddge::util::meta
