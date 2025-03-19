module;

#include <type_traits>

export module core.ecs:Query.fwd;

import utility.meta.type_traits.all_different;

import :queryable_component_c;

namespace core::ecs {

export template <typename... Components_T>
    requires(queryable_component_c<std::remove_const_t<Components_T>> && ...)
         && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
class Query;

}   // namespace core::ecs
