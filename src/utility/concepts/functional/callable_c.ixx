module;

export module utility.concepts.functional.callable_c;

import utility.concepts.functional.function_c;
import utility.concepts.functional.functor_c;
import utility.concepts.functional.member_function_pointer_c;

namespace util::meta {

export template <typename Callable>
concept callable_c = function_c<Callable> || member_function_pointer_c<Callable> || functor_c<Callable>;

}   // namespace util::meta
