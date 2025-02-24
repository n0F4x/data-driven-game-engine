module;

#include <entt/core/type_info.hpp>

export module utility.meta.reflection.hash;

namespace util::meta {

// TODO: use proper reflection

export using hash_type = entt::id_type;

export template <typename T>
constexpr bool hash_v = entt::type_hash<T>::value();

}   // namespace util::meta
