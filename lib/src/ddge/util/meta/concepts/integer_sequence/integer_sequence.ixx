export module ddge.util.meta.concepts.integer_sequence.integer_sequence;

import ddge.util.meta.type_traits.integer_sequence.is_integer_sequence;

namespace ddge::util::meta {

export template <typename T>
concept integer_sequence_c = is_integer_sequence_v<T>;

}   // namespace ddge::util::meta
