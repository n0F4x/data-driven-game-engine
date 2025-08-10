module;

#include <entt/core/type_info.hpp>

export module ddge.utility.meta.reflection.name_of;

namespace ddge::util::meta {

// TODO: use reflection

export template <typename T>
inline constexpr auto name_of =
    [] static -> std::string_view { return entt::type_name<T>::value(); };

}   // namespace ddge::util::meta
