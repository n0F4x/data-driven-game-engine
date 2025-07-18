module;

#include <type_traits>

export module core.assets.loader_c;

import core.assets.asset_c;

import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.concepts.hashable;
import utility.meta.concepts.naked;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_transform;

template <typename T>
struct Hashable : std::bool_constant<util::meta::hashable_c<T>> {};

namespace core::assets {

export template <typename T>
concept loader_c =
    util::meta::naked_c<T>   //
    && asset_c<util::meta::result_of_t<T>>
    && util::meta::type_list_all_of_c<
        util::meta::type_list_transform_t<util::meta::arguments_of_t<T>, std::remove_cvref>,
        ::Hashable>;

}   // namespace core::assets
