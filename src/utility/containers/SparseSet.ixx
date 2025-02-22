module;

#include <cstdint>

export module utility.containers.SparseSet;

import utility.containers.sparse_set.SparseSet;
import utility.containers.sparse_set.key_c;
import utility.containers.sparse_set.value_c;

namespace util {

export template <
    util::sparse_set::key_c   Key_T,
    util::sparse_set::value_c T,
    uint8_t                   version_bits_T = sizeof(Key_T) * 2>
using SparseSet = sparse_set::SparseSet<Key_T, T, version_bits_T>;

}   // namespace util
