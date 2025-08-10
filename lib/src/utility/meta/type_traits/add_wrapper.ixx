export module ddge.utility.meta.type_traits.add_wrapper;

import ddge.utility.meta.type_traits.underlying;

namespace ddge::util::meta {

export template <typename T, template <typename> typename Wrapper_T>
struct add_wrapper {
    using type = Wrapper_T<T>;
};

export template <typename T, template <typename> typename Wrapper_T>
using add_wrapper_t = typename add_wrapper<T, Wrapper_T>::type;

}   // namespace ddge::util::meta
