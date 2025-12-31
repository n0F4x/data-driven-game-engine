module;

#include <type_traits>

export module ddge.utility.meta.concepts.preserves_const;

namespace ddge::util::meta {

export template <typename To, typename From>
concept preserves_const_c = !(std::is_const_v<From> && not std::is_const_v<To>);

}   // namespace ddge::util::meta
