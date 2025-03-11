module;

export module utility.meta.concepts.functional.callable;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.functor;
import utility.meta.concepts.functional.member_function_pointer;

namespace util::meta {

export template <typename Callable>
concept callable_c = function_c<Callable> || member_function_pointer_c<Callable> || functor_c<Callable>;

}   // namespace util::meta
