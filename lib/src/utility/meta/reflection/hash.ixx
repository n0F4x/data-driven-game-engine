module;

#include <entt/core/type_info.hpp>

export module ddge.utility.meta.reflection.hash;

namespace ddge::util::meta {

// TODO: use reflection

export using TypeHash = entt::id_type;

export template <typename T>
inline constexpr auto hash = [] static -> TypeHash { return entt::type_hash<T>::value(); };

}   // namespace ddge::util::meta
