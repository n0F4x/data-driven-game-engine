module;

#include <tuple>

export module ddge.utility.tuple.tuple_select;

namespace ddge::util {

export template <typename Tuple_T, std::size_t... Ints_V>
auto tuple_select(Tuple_T&& tuple, std::index_sequence<Ints_V...>)
{
    return std::tuple<std::tuple_element_t<Ints_V, Tuple_T>...>{
        std::get<Ints_V>(std::forward<Tuple_T>(tuple))...
    };
}

}   // namespace ddge::util
