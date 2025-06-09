module;

#include <optional>
#include <type_traits>

export module core.ecs:query.query_parameter_tags;

import utility.containers.OptionalRef;

import :component_c;
import :query.QueryParameterTagBase;

namespace core::ecs::inline query_parameter_tags {

export template <component_c>
struct With : ::QueryParameterTagBase {};

export template <component_c>
struct Without : ::QueryParameterTagBase {};

export template <typename T>
    requires component_c<std::remove_const_t<T>>
struct Optional : ::QueryParameterTagBase, util::OptionalRef<T> {
    using util::OptionalRef<T>::OptionalRef;
    using util::OptionalRef<T>::operator std::optional<std::remove_const_t<T>>;
};

// TODO: remove deduction guide with P2582
template <typename T>
Optional(T&) -> Optional<T>;

}   // namespace core::ecs::inline query_parameter_tags
