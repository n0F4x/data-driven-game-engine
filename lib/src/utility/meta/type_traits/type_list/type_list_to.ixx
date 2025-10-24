export module ddge.utility.meta.type_traits.type_list.type_list_to;

namespace ddge::util::meta {

export template <typename From_T, template <typename...> typename To_T>
struct type_list_to;

template <template <typename...> typename From_T, typename... Ts, template <typename...> typename To_T>
struct type_list_to<From_T<Ts...>, To_T> {
    using type = To_T<Ts...>;
};

export template <typename From_T, template <typename...> typename To_T>
using type_list_to_t = typename type_list_to<From_T, To_T>::type;

}   // namespace ddge::util::meta
