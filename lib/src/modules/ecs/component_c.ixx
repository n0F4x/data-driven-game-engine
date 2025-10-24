module;

#include <concepts>
#include <type_traits>

export module ddge.modules.ecs:component_c;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.nothrow_movable;
import ddge.utility.meta.concepts.storable;

import :ID;
import :query.query_filter_tag_c;

namespace ddge::ecs {

export template <typename T>
concept component_c = util::meta::naked_c<T> && util::meta::storable_c<T>
                   && util::meta::nothrow_movable_c<T> && !std::same_as<T, ID>
                   && !query_filter_tag_c<T>;

}   // namespace ddge::ecs
