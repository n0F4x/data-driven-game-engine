export module ddge.modules.ecs:query.QueryClosure.fwd;

import :query.query_filter_c;
import :query.query_filter_components_are_all_different_c;

namespace ddge::ecs {

export template <ddge::ecs::query_filter_c... Filters_T>
    requires ::query_filter_components_are_all_different_c<Filters_T...>
class Query;

}   // namespace ddge::ecs
