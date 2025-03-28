export module core.ecs:query.QueryAdaptorClosure.fwd;

import :query.query_parameter_c;
import :query.query_parameter_components_are_all_different_c;

namespace core::ecs {

template <query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
class QueryAdaptorClosure;

}   // namespace core::ecs
