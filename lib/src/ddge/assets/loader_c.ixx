module;

#include <concepts>
#include <type_traits>

export module ddge.assets.loader_c;

import ddge.assets.asset_c;

import ddge.util.meta.concepts.functional.unambiguously_invocable;
import ddge.util.meta.concepts.hashable;
import ddge.util.meta.concepts.naked;
import ddge.util.meta.concepts.storable;
import ddge.util.meta.concepts.type_list.type_list_all_of;
import ddge.util.meta.type_traits.functional.arguments_of;
import ddge.util.meta.type_traits.functional.result_of;
import ddge.util.meta.type_traits.type_list.type_list_transform;

template <typename T>
struct Hashable : std::bool_constant<ddge::util::meta::hashable_c<T>> {};

namespace ddge::assets {

export template <typename T>
concept loader_c =
    util::meta::storable_c<std::decay_t<T>> && util::meta::naked_c<T>
    && std::movable<std::decay_t<T>>   //
    && asset_c<util::meta::result_of_t<T>>
    && util::meta::type_list_all_of_c<
        util::meta::type_list_transform_t<util::meta::arguments_of_t<T>, std::remove_cvref>,
        ::Hashable>;

}   // namespace ddge::assets
