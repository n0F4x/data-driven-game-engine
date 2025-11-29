module;

#include <type_traits>

export module ddge.utility.meta.type_traits.volatile_like;

namespace ddge::util::meta {

export template <typename T, typename Like_T>
using volatile_like_t = std::conditional_t<
    std::is_volatile_v<Like_T>,
    std::add_volatile_t<T>,
    std::remove_volatile_t<T>>;

}   // namespace ddge::util::meta
