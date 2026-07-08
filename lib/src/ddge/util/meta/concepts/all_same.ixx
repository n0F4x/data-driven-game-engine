export module ddge.util.meta.concepts.all_same;

import ddge.util.meta.type_traits.all_same;

namespace ddge::util::meta {

export template <typename... Ts>
concept all_same_c = all_same_v<Ts...>;

}   // namespace ddge::util::meta
