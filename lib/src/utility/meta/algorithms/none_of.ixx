export module utility.meta.algorithms.none_of;

import utility.meta.algorithms.any_of;

namespace util::meta {

export template <typename T>
constexpr inline auto none_of = []<typename Predicate_T>(Predicate_T predicate) -> bool {
    return !any_of<T>(predicate);
};

}   // namespace util::meta
