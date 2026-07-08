module;

#include <tuple>

export module ddge.util.tuple.tuple_select;

namespace ddge::util {

export template <typename Tuple_T, std::size_t... indices_T>
auto tuple_select(Tuple_T&& tuple, std::index_sequence<indices_T...>)
{
    return std::tuple<std::tuple_element_t<indices_T, Tuple_T>...>{
        std::get<indices_T>(std::forward<Tuple_T>(tuple))...
    };
}

}   // namespace ddge::util
