module;

#include <type_traits>

export module core.ecs:fwd;

import utility.meta.concepts.nothrow_movable;
import utility.meta.type_traits.all_different;

namespace core::ecs {

export template <typename T>
concept component_c = !std::is_const_v<T> && util::meta::nothrow_movable_c<T>;

export template <typename T>
concept queryable_component_c = component_c<T> && !std::is_empty_v<T>;

export template <typename... Components_T>
    requires(queryable_component_c<std::remove_const_t<Components_T>> && ...)
         && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
class Query;

export template <auto tag_T>
class Registry;

}   // namespace core::ecs
