module;

#include <entt/core/type_info.hpp>

export module utility.meta.reflection.type_hash;

namespace util::meta {

// TODO: use proper reflection

export using TypeHash = entt::id_type;

export template <typename T>
constexpr TypeHash hash_v = entt::type_hash<T>::value();

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

using MyInt = int;
static_assert(util::meta::hash_v<int> == util::meta::hash_v<MyInt>);

#endif
