module;

#include <type_traits>

export module utility.meta.concepts.functional.function_pointer;

namespace util::meta {

export template <typename T>
concept function_pointer_c = std::is_pointer_v<T>
                          && std::is_function_v<std::remove_pointer_t<T>>;

}   // namespace util::meta
