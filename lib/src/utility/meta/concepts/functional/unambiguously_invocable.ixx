module;

#include <type_traits>

export module utility.meta.concepts.functional.unambiguously_invocable;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.function_pointer;
import utility.meta.concepts.functional.function_reference;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_functor;
import utility.meta.concepts.functional.unambiguous_explicit_functor;

namespace util::meta {

export template <typename T>
concept unambiguously_invocable_c = function_c<T>                                    //
                                 || function_pointer_c<std::remove_reference_t<T>>   //
                                 || function_reference_c<T>
                                 || member_function_pointer_c<T>
                                 || unambiguous_functor_c<T>;

}   // namespace util::meta
