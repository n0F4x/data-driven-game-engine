module;

#include <optional>
#include <type_traits>

export module ddge.modules.ecs:query.query_filter_tags;

import ddge.utility.containers.OptionalRef;

import :component_c;
import :query.QueryFilterTagBase;

namespace ddge::ecs::inline query_filter_tags {

export template <component_c>
struct With : ::QueryFilterTagBase {};

export template <component_c>
struct Without : ::QueryFilterTagBase {};

export template <typename T>
    requires component_c<std::remove_const_t<T>>
struct Optional : ::QueryFilterTagBase, util::OptionalRef<T> {
    using util::OptionalRef<T>::OptionalRef;
    using util::OptionalRef<T>::operator std::optional<std::remove_const_t<T>>;
};

// TODO: remove deduction guide with P2582
template <typename T>
Optional(T&) -> Optional<T>;

}   // namespace ddge::ecs::inline query_filter_tags
