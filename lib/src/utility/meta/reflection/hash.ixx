module;

#include <entt/core/type_info.hpp>

export module utility.meta.reflection.hash;

namespace util::meta {

// TODO: use reflection

export using TypeHash = entt::id_type;

export template <typename T>
constexpr inline auto hash = [] static -> TypeHash { return entt::type_hash<T>::value(); };

}   // namespace util::meta
