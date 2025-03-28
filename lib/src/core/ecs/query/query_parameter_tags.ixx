module;

#include <type_traits>

export module core.ecs:query.query_parameter_tags;

import :component_c;
import :query.query_parameter_tags.fwd;

namespace core::ecs::inline query_parameter_tags {

export template <component_c T>
struct With<T> {};

export template <typename T>
    requires(!component_c<T>)
struct With<T> {
    static_assert(false, "invalid specialization");
};

export template <component_c T>
struct Without<T> {};

export template <typename T>
    requires(!component_c<T>)
struct Without<T> {
    static_assert(false, "invalid specialization");
};

export template <typename T>
    requires component_c<std::remove_const_t<T>>
struct Optional<T> {};

export template <typename T>
    requires(!component_c<std::remove_const_t<T>>)
struct Optional<T> {
    static_assert(false, "invalid specialization");
};

}   // namespace core::ecs::inline query_parameter_tags
