module;

#include <type_traits>

export module utility.meta.concepts.functional.function_reference;

namespace util::meta {

export template <typename T>
concept function_reference_c = std::is_reference_v<T>
                            && std::is_function_v<std::remove_reference_t<T>>;

static_assert(!function_reference_c<void()>);
static_assert(function_reference_c<void (&)()>);
static_assert(!function_reference_c<void (*)()>);

}   // namespace util::meta
