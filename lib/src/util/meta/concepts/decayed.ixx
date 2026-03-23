module;

#include <type_traits>

export module ddge.util.meta.concepts.decayed;

import ddge.util.meta.concepts.decays_to;

namespace ddge::util::meta {

export template <typename T>
concept decayed_c = decays_to_c<T, std::decay_t<T>>;

}   // namespace ddge::util::meta
