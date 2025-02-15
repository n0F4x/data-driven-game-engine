export module utility.meta.concepts.index_sequence_c;

import utility.meta.type_traits.integer_sequence.is_index_sequence;

namespace util::meta {

export template<typename T>
concept intex_sequence_c = is_index_sequence_v<T>;

}
