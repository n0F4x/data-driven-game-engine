module;

#include <type_traits>

export module ddge.utility.meta.concepts.functional.unambiguously_invocable;

import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.function_pointer;
import ddge.utility.meta.concepts.functional.function_reference;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.functional.unambiguous_functor;
import ddge.utility.meta.concepts.functional.unambiguous_explicit_functor;

namespace ddge::util::meta {

export template <typename T>
concept unambiguously_invocable_c = function_c<T>                                    //
                                 || function_pointer_c<std::remove_reference_t<T>>   //
                                 || function_reference_c<T>
                                 || member_function_pointer_c<T>
                                 || unambiguous_functor_c<T>;

}   // namespace ddge::util::meta
