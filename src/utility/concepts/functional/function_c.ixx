module;

#include <type_traits>

export module utility.concepts.functional.function_c;

namespace util::meta {

export template <typename F>
concept function_c = std::is_function_v<F>;

}   // namespace util::meta
