module;

#include <array>

export module ddge.utility.meta.type_traits.type_list.type_list_index_of;

template <typename, typename>
struct type_list_index_of_impl;

constexpr static std::size_t not_found = static_cast<std::size_t>(-1);
constexpr static std::size_t ambiguous = not_found - 1;

[[nodiscard]]
consteval auto find_index_return(
    const std::size_t current_index,
    const std::size_t result,
    const bool        current_match
) -> std::size_t
{
    return !current_match ? result : (result == not_found ? current_index : ambiguous);
}

template <std::size_t N>
[[nodiscard]]
consteval auto
    find_index(const std::size_t current_index, const std::array<bool, N> matches)
        -> std::size_t
{
    return current_index == N ? not_found
                              : find_index_return(
                                    current_index,
                                    find_index(current_index + 1, matches),
                                    matches[current_index]
                                );
}

template <template <typename...> typename TypeList_T, typename... Ts, typename T>
struct type_list_index_of_impl<TypeList_T<Ts...>, T> {
    constexpr static std::array<bool, sizeof...(Ts)> matches{ std::is_same_v<T, Ts>... };
    constexpr static std::size_t                     value{ find_index(0, matches) };
    static_assert(value != not_found, "type not found in type list");
    static_assert(value != ambiguous, "type occurs more than once in type list");
};

namespace ddge::util::meta {

template <typename TypeList_T, typename T>
struct type_list_index_of {
    constexpr static std::size_t value = ::type_list_index_of_impl<TypeList_T, T>::value;
};

export template <typename TypeList_T, typename T>
inline constexpr std::size_t type_list_index_of_v =
    type_list_index_of<TypeList_T, T>::value;

}   // namespace ddge::util::meta
