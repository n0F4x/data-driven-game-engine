export module utility.concepts.integer_sequence_c;

import utility.type_traits.integer_sequence.is_integer_sequence;

namespace util::meta {

export template<typename T>
concept integer_sequence_c = is_integer_sequence_v<T>;

}
