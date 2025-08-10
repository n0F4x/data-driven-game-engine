module;

#include <type_traits>

export module ddge.utility.meta.concepts.storable;

namespace ddge::util::meta {

export template <typename T>
concept storable_c = std::is_nothrow_destructible_v<T>;

}   // namespace ddge::util::meta
