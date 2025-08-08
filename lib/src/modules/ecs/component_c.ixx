module;

#include <concepts>
#include <type_traits>

export module modules.ecs:component_c;

import utility.meta.concepts.nothrow_movable;

import :ID;
import :query.query_parameter_tag_c;

namespace modules::ecs {

export template <typename T>
concept component_c = !std::is_const_v<T> && util::meta::nothrow_movable_c<T>
                   && !std::same_as<T, ID> && !query_parameter_tag_c<T>;

}   // namespace modules::ecs
