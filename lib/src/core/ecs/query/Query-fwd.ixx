module;

#include <tuple>
#include <type_traits>

export module core.ecs:query.Query.fwd;

import utility.meta.concepts.type_list.type_list_all_different;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;

import :query.query_parameter_tags;
import :query.queryable_component_c;

template <typename T>
struct ToRawComponent : std::type_identity<std::remove_const_t<T>> {};

template <core::ecs::query_parameter_tag_c T>
struct ToRawComponent<T>
    : std::type_identity<std::remove_const_t<util::meta::underlying_t<T>>> {};

template <typename... QueryParameters_T>
concept query_parameter_components_are_all_different_c =
    util::meta::type_list_all_different_c<
        util::meta::type_list_transform_t<std::tuple<QueryParameters_T...>, ToRawComponent>>;

namespace core::ecs {

export template <typename T>
concept query_parameter_c = queryable_component_c<std::remove_const_t<T>>
                         || query_parameter_tag_c<T>;

export template <query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
class Query;

}   // namespace core::ecs
