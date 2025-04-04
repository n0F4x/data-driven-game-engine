export module utility.meta.type_traits.type_list.type_list_chunk_sub;

import utility.meta.concepts.type_list.type_list_all_of;

import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_disjoin;
import utility.meta.type_traits.type_list.type_list_join;
import utility.meta.type_traits.is_specialization_of;

namespace util::meta {

template <template <typename> typename Wrapper_T>
struct is_specialization_of_Wrapper {
    template <typename T>
    struct trait : is_specialization_of<T, Wrapper_T> {};
};

template <
    template <typename...> typename SubTypeList_T,
    template <typename...> typename Transform_T>
struct transform_join_if_list_of_subtype_lists {
    template <typename TypeList_T>
    struct trait {
        using type = TypeList_T;
    };

    template <typename TypeList_T>
        requires type_list_all_of_c<
            TypeList_T,
            is_specialization_of_Wrapper<SubTypeList_T>::template trait>
    struct trait<TypeList_T>
        : type_list_transform<type_list_join_t<TypeList_T>, Transform_T> {};
};

template <template <typename...> typename SubTypeList_T>
struct add_subtype_list {
    template <typename T>
    struct trait {
        using type = SubTypeList_T<T>;
    };
};

export template <typename TypeList_T, template <typename...> typename SubTypeList_T>
struct type_list_chunk_sub {
    using type = type_list_join_t<type_list_transform_t<
        type_list_disjoin_t<TypeList_T>,
        transform_join_if_list_of_subtype_lists<
            SubTypeList_T,
            add_subtype_list<SubTypeList_T>::template trait>::template trait>>;
};

export template <typename TypeList_T, template <typename...> typename SubTypeList_T>
using type_list_chunk_sub_t =
    typename type_list_chunk_sub<TypeList_T, SubTypeList_T>::type;

}   // namespace util::meta
