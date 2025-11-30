export module ddge.utility.meta.concepts.all_same;

import ddge.utility.meta.type_traits.all_same;

namespace ddge::util::meta {

export template <typename... Ts>
concept all_same_c = all_same_v<Ts...>;

}   // namespace ddge::util::meta
