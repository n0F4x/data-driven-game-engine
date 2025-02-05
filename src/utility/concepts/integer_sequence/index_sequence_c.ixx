export module utility.concepts.index_sequence_c;

import utility.type_traits.integer_sequence.is_index_sequence;

namespace util::meta {

export template<typename T>
concept intex_sequence_c = is_index_sequence_v<T>;

}
