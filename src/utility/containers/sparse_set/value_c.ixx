module;

#include <type_traits>

export module utility.containers.sparse_set.value_c;

import utility.meta.concepts.nothrow_movable;

namespace util::sparse_set {

export template <typename T>
concept value_c = !std::is_const_v<T> && meta::nothrow_movable_c<T>;

}   // namespace util::sparse_set
