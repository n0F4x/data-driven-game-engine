module;

#include <type_traits>

export module ddge.utility.meta.type_traits.remove_wrapper_if;

import ddge.utility.meta.type_traits.underlying;

namespace ddge::util::meta {

export template <typename T, template <typename> typename Predicate_T>
struct remove_wrapper_if {
    using type = std::conditional_t<Predicate_T<T>::value, underlying_t<T>, T>;
};

export template <typename T, template <typename> typename Predicate_T>
using remove_wrapper_if_t = typename remove_wrapper_if<T, Predicate_T>::type;

}   // namespace ddge::util::meta
