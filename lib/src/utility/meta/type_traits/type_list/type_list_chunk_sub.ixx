export module ddge.utility.meta.type_traits.type_list.type_list_chunk_sub;

import ddge.utility.meta.concepts.type_list.type_list_all_of;

import ddge.utility.meta.type_traits.type_list.type_list_disjoin;
import ddge.utility.meta.type_traits.type_list.type_list_join;
import ddge.utility.meta.type_traits.type_list.type_list_to;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.meta.type_traits.is_specialization_of;

template <template <typename> typename Wrapper_T>
struct is_specialization_of_Wrapper {
    template <typename T>
    struct trait : ddge::util::meta::is_specialization_of<T, Wrapper_T> {};
};

template <
    template <typename...> typename SubTypeList_T,
    template <typename...> typename Transform_T>
struct transform_join_if_list_of_subtype_lists {
    template <typename TypeList_T>
    struct trait {
        using type = TypeList_T;
    };

    template <template <typename...> typename TypeList_T, typename... Ts>
        requires ddge::util::meta::type_list_all_of_c<
            TypeList_T<Ts...>,
            is_specialization_of_Wrapper<SubTypeList_T>::template trait>
    struct trait<TypeList_T<Ts...>>
        : ddge::util::meta::type_list_to<
              ddge::util::meta::type_list_transform_t<
                  ddge::util::meta::type_list_join_t<TypeList_T<Ts...>>,
                  Transform_T>,
              TypeList_T> {};
};

template <template <typename...> typename SubTypeList_T>
struct add_subtype_list {
    template <typename T>
    struct trait {
        using type = SubTypeList_T<T>;
    };
};

template <typename TypeList_T, template <typename...> typename SubTypeList_T>
struct type_list_chunk_sub_impl;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename...> typename SubTypeList_T>
struct type_list_chunk_sub_impl<TypeList_T<Ts...>, SubTypeList_T> {
    using type =
        ddge::util::meta::type_list_join_t<ddge::util::meta::type_list_transform_t<
            ddge::util::meta::type_list_disjoin_t<TypeList_T<Ts...>>,
            transform_join_if_list_of_subtype_lists<
                SubTypeList_T,
                add_subtype_list<SubTypeList_T>::template trait>::template trait>>;
};

namespace ddge::util::meta {

export template <typename TypeList_T, template <typename...> typename SubTypeList_T>
struct type_list_chunk_sub {
    using type = typename type_list_chunk_sub_impl<TypeList_T, SubTypeList_T>::type;
};

export template <typename TypeList_T, template <typename...> typename SubTypeList_T>
using type_list_chunk_sub_t =
    typename type_list_chunk_sub<TypeList_T, SubTypeList_T>::type;

}   // namespace ddge::util::meta
