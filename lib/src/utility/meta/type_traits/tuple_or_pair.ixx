module;

#include <type_traits>
#include <utility>

export module ddge.utility.meta.type_traits.tuple_or_pair;

namespace ddge::util::meta {

export template <typename... Ts>
using tuple_or_pair_t = std::conditional_t < sizeof...(Ts) == 2,
      std::pair<Ts...>, std::tuple < Ts... >> ;

}   // namespace ddge::util::meta
