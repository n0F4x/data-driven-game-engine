module;

#include <type_traits>
#include <utility>

export module ddge.utility.meta.type_traits.mimic;

import ddge.utility.meta.type_traits.const_like;
import ddge.utility.meta.type_traits.reference_like;
import ddge.utility.meta.type_traits.volatile_like;

namespace ddge::util::meta {

export template <typename T, typename Like_T>
using mimic_t = reference_like_t<
    const_like_t<
        volatile_like_t<std::remove_cvref_t<T>, std::remove_reference_t<Like_T>>,
        std::remove_reference_t<Like_T>>,
    Like_T>;

}   // namespace ddge::util::meta
