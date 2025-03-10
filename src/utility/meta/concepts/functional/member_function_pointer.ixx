module;

#include <type_traits>

export module utility.meta.concepts.functional.member_function_pointer;

namespace util::meta {

export template <typename F>
concept member_function_pointer_c = std::is_member_function_pointer_v<F>;

}   // namespace util::meta
