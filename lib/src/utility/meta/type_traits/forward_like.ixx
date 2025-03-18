module;

#include <utility>

export module utility.meta.type_traits.forward_like;

namespace util::meta {

export template <typename T, typename Like_T>
using forward_like_t = decltype(std::forward_like<Like_T>(std::declval<T>()));

}   // namespace util::meta
