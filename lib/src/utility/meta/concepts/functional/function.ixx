module;

#include <type_traits>

export module ddge.utility.meta.concepts.functional.function;

namespace ddge::util::meta {

export template <typename F>
concept function_c = std::is_function_v<F>;

}   // namespace ddge::util::meta
