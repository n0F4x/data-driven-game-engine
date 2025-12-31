module;

#include <type_traits>

export module ddge.utility.meta.concepts.pointer;

namespace ddge::util::meta {

export template <typename T>
concept pointer_c = std::is_pointer_v<T>;

}   // namespace ddge::util::meta
