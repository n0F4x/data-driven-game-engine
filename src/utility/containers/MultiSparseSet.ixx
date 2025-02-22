module;

export module utility.containers.MultiSparseSet;

import utility.containers.sparse_set.MultiSparseSet;

namespace util {

// export template <
//     util::sparse_set::key_c   Key_T,
//     util::sparse_set::value_c T,
//     uint8_t                   version_bits_T = sizeof(Key_T) * 2>
// using MultiSparseSet = sparse_set::MultiSparseSet<Key_T, T, version_bits_T>;

export using sparse_set::MultiSparseSet;

}   // namespace util
