module;

#include <entt/core/type_info.hpp>

export module utility.meta.reflection.type_id;

namespace util::meta {

export using TypeID = entt::id_type;

export template <typename T>
constexpr TypeID id_v{ entt::type_hash<T>::value() };

}   // namespace util::meta
