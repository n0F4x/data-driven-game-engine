module;

#include <array>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <tuple>
#endif

export module utility.meta.type_traits.type_list.type_list_index_of;

template <typename, typename>
struct type_list_index_of_impl;

constexpr static size_t not_found = static_cast<size_t>(-1);
constexpr static size_t ambiguous = not_found - 1;

[[nodiscard]]
consteval auto find_index_return(
    const size_t current_index,
    const size_t result,
    const bool   current_match
) -> size_t
{
    return !current_match ? result : (result == not_found ? current_index : ambiguous);
}

template <size_t N>
[[nodiscard]]
consteval auto find_index(const size_t current_index, const std::array<bool, N> matches)
    -> size_t
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
    constexpr static size_t                          value{ find_index(0, matches) };
    static_assert(value != not_found, "type not found in type list");
    static_assert(value != ambiguous, "type occurs more than once in type list");
};

namespace util::meta {

template <typename TypeList_T, typename T>
struct type_list_index_of {
    constexpr static size_t value = ::type_list_index_of_impl<TypeList_T, T>::value;
};

export template <typename TypeList_T, typename T>
constexpr inline size_t type_list_index_of_v = type_list_index_of<TypeList_T, T>::value;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(util::meta::type_list_index_of_v<std::tuple<int, float, long>, int> == 0);
static_assert(util::meta::type_list_index_of_v<std::tuple<int, float, long>, float> == 1);
static_assert(util::meta::type_list_index_of_v<std::tuple<int, float, long>, long> == 2);

#endif
