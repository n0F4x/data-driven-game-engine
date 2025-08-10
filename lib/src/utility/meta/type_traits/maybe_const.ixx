module;

#include <type_traits>

export module ddge.utility.meta.type_traits.maybe_const;

namespace ddge::util::meta {

export template <bool is_const_T, typename T>
using maybe_const_t = std::conditional_t<is_const_T, const T, T>;

}   // namespace ddge::util::meta
