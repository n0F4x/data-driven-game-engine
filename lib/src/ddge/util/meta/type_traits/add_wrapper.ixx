export module ddge.util.meta.type_traits.add_wrapper;

import ddge.util.meta.type_traits.underlying;

namespace ddge::util::meta {

export template <typename T, template <typename> typename Wrapper_T>
struct add_wrapper {
    using type = Wrapper_T<T>;
};

export template <typename T, template <typename> typename Wrapper_T>
using add_wrapper_t = Wrapper_T<T>;

}   // namespace ddge::util::meta
