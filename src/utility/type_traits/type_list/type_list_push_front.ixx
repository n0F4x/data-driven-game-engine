module;

export module utility.type_traits.type_list.type_list_push_front;

namespace util::meta {

export template <typename TypeList_T, typename T>
struct type_list_push_front;

template <template <typename...> typename TypeList_T, typename... Ts, typename T>
struct type_list_push_front<TypeList_T<Ts...>, T> {
    using type = TypeList_T<T, Ts...>;
};

export template <typename TypeList_T, typename T>
using type_list_push_front_t = typename type_list_push_front<TypeList_T, T>::type;

}   // namespace util::meta
