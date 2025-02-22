module;

#include <concepts>
#include <type_traits>

export module utility.containers.sparse_set.key_c;

namespace util::sparse_set {

export template <typename T>
concept key_c = std::unsigned_integral<T> && !std::is_const_v<T>;

}   // namespace util::sparse_set
