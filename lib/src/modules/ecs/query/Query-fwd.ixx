export module modules.ecs:query.QueryClosure.fwd;

import :query.query_parameter_c;
import :query.query_parameter_components_are_all_different_c;

namespace modules::ecs {

export template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
class Query;

}   // namespace modules::ecs
