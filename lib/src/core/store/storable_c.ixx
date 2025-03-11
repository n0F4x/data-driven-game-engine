module;

#include <concepts>

export module core.store.storable_c;

namespace core::store {

export template <typename T>
concept storable_c = std::movable<T>;

}   // namespace core::store
