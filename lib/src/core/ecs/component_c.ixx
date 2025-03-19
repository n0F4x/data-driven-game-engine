module;

#include <type_traits>

export module core.ecs:component_c;

import utility.meta.concepts.nothrow_movable;

namespace core::ecs {

export template <typename T>
concept component_c = !std::is_const_v<T> && util::meta::nothrow_movable_c<T>;

}   // namespace core::ecs

template <typename T>
concept decays_to_component_c = core::ecs::component_c<std::decay_t<T>>;
