export module core.ecs:query.query_parameter_tag_c;

import utility.meta.concepts.specialization_of;

import :query.query_parameter_tags.fwd;

namespace core::ecs {

export template <typename T>
concept query_parameter_tag_c = util::meta::specialization_of_c<T, core::ecs::With>
    || util::meta::specialization_of_c<T, core::ecs::Without>
    || util::meta::specialization_of_c<T, core::ecs::Optional>;

}   // namespace core::ecs
