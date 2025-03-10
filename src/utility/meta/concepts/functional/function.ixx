module;

#include <type_traits>

export module utility.meta.concepts.functional.function;

namespace util::meta {

export template <typename F>
concept function_c = std::is_function_v<F>;

}   // namespace util::meta
