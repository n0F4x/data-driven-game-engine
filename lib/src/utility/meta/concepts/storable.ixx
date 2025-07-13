module;

#include <type_traits>

export module utility.meta.concepts.storable;

namespace util::meta {

export template <typename T>
concept storable_c = std::is_nothrow_destructible_v<T>;

}   // namespace util::meta
