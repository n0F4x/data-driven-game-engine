module;

#include <concepts>
#include <type_traits>

export module ddge.modules.ecs:query.query_parameter_c;

import :ID;
import :query.queryable_component_c;
import :query.query_filter_tags;

import ddge.utility.meta.concepts.specialization_of;

namespace ddge::ecs {

export template <typename T>
concept query_parameter_c =
    !std::is_rvalue_reference_v<T>
    && (ddge::ecs::queryable_component_c<std::remove_cvref_t<T>>
        || ddge::util::meta::
            specialization_of_c<std::remove_cvref_t<T>, query_filter_tags::Optional>
        || std::same_as<std::remove_cvref_t<T>, ID>);

}   // namespace ddge::ecs
