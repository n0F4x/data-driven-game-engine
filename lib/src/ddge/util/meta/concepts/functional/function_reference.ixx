module;

#include <type_traits>

export module ddge.util.meta.concepts.functional.function_reference;

namespace ddge::util::meta {

export template <typename T>
concept function_reference_c = std::is_reference_v<T>
                            && std::is_function_v<std::remove_reference_t<T>>;

}   // namespace ddge::util::meta
