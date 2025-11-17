module;

#include <type_traits>

export module ddge.utility.meta.type_traits.reference_like;

namespace ddge::util::meta {

export template <typename T, typename Like_T>
using reference_like_t = std::conditional_t<
    std::is_lvalue_reference_v<Like_T>,
    std::add_lvalue_reference_t<std::remove_reference_t<T>>,
    std::conditional_t<
        std::is_rvalue_reference_v<Like_T>,
        std::add_rvalue_reference_t<std::remove_reference_t<T>>,
        std::remove_reference_t<T>>>;

}   // namespace ddge::util::meta
