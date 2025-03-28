export module core.ecs:query.query;

import :query.QueryAdaptorClosure;

namespace core::ecs {

export template <query_parameter_c... Parameters_T>
constexpr inline QueryAdaptorClosure<Parameters_T...> query;

}   // namespace core::ecs
