module;

#include <type_traits>

export module ddge.utility.meta.concepts.functional.function_pointer;

namespace ddge::util::meta {

export template <typename T>
concept function_pointer_c = std::is_pointer_v<T>
                          && std::is_function_v<std::remove_pointer_t<T>>;

}   // namespace ddge::util::meta
