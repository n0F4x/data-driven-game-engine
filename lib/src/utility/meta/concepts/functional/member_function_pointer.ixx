module;

#include <type_traits>

export module ddge.utility.meta.concepts.functional.member_function_pointer;

namespace ddge::util::meta {

export template <typename F>
concept member_function_pointer_c = std::is_member_function_pointer_v<F>;

}   // namespace ddge::util::meta
