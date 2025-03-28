module;

#include <entt/core/type_info.hpp>

export module utility.meta.reflection.name_of;

namespace util::meta {

// TODO: use reflection

export template <typename T>
constexpr inline std::string_view name_of{ entt::type_name<T>::value() };

}   // namespace util::meta
