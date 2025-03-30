export module core.ecs:query.query;

import :query.QueryClosure;

namespace core::ecs {

export template <query_parameter_c... Parameters_T>
constexpr inline QueryClosure<Parameters_T...> query;

}   // namespace core::ecs
