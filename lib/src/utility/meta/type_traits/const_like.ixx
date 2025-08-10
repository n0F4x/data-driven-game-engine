module;

#include <type_traits>

export module ddge.utility.meta.type_traits.const_like;

namespace ddge::util::meta {

export template <typename T, typename Like_T>
using const_like_t = std::
    conditional_t<std::is_const_v<Like_T>, std::add_const_t<T>, std::remove_const_t<T>>;

}   // namespace ddge::util::meta
