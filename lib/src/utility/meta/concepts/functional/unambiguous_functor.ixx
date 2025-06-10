export module utility.meta.concepts.functional.unambiguous_functor;

import utility.meta.concepts.functional.unambiguous_explicit_functor;
import utility.meta.concepts.functional.unambiguous_implicit_functor;

namespace util::meta {

export template <typename T>
concept unambiguous_functor_c = unambiguous_explicit_functor_c<T>
                             || unambiguous_implicit_functor_c<T>;

}   // namespace util::meta
