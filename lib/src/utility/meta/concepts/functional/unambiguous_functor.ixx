export module ddge.utility.meta.concepts.functional.unambiguous_functor;

import ddge.utility.meta.concepts.functional.unambiguous_explicit_functor;
import ddge.utility.meta.concepts.functional.unambiguous_implicit_functor;
import ddge.utility.meta.concepts.functional.unambiguous_static_functor;

namespace ddge::util::meta {

export template <typename T>
concept unambiguous_functor_c = unambiguous_explicit_functor_c<T>
                             || unambiguous_implicit_functor_c<T>
                             || unambiguous_static_functor_c<T>;

}   // namespace ddge::util::meta
