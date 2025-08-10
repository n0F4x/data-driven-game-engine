export module ddge.utility.meta.type_traits.type_list.type_list_transform;

import ddge.utility.meta.type_traits.type_list.type_list_front;

namespace ddge::util::meta {

export template <typename TypeList_T, template <typename> typename Transform_T>
struct type_list_transform;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Transform_T>
struct type_list_transform<TypeList_T<Ts...>, Transform_T> {
    using type = TypeList_T<typename Transform_T<Ts>::type...>;
};

export template <typename TypeList_T, template <typename> typename Transform_T>
using type_list_transform_t = typename type_list_transform<TypeList_T, Transform_T>::type;

}   // namespace ddge::util::meta
