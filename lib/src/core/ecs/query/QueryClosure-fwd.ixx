export module core.ecs:query.QueryClosure.fwd;

import :query.query_parameter_c;
import :query.query_parameter_components_are_all_different_c;

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
struct QueryClosure;
