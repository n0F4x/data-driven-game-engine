export module ddge.utility.meta.algorithms.none_of;

import ddge.utility.meta.algorithms.any_of;

namespace ddge::util::meta {

export template <typename T, typename Predicate_T>
constexpr auto none_of(Predicate_T predicate) -> bool
{
    return !any_of<T>(predicate);
}

}   // namespace ddge::util::meta
