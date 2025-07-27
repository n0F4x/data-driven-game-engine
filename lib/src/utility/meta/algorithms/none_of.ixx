export module utility.meta.algorithms.none_of;

import utility.meta.algorithms.any_of;

namespace util::meta {

export template <typename T, typename Predicate_T>
constexpr auto none_of(Predicate_T predicate) -> bool
{
    return !any_of<T>(predicate);
}

}   // namespace util::meta
