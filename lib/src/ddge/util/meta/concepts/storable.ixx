module;

#include <concepts>
#include <type_traits>

export module ddge.util.meta.concepts.storable;

namespace ddge::util::meta {

export template <typename T>
concept storable_c = !std::is_abstract_v<T> && std::destructible<T>;

}   // namespace ddge::util::meta
