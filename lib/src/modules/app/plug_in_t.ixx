module;

#include <utility>

export module ddge.modules.app.plug_in_t;

import ddge.modules.app.Builder;
import ddge.modules.app.decays_to_builder_c;
import ddge.modules.app.plugin_c;

import ddge.utility.meta.type_traits.type_list.type_list_concat;

namespace ddge::app {

export template <decays_to_builder_c Builder_T, plugin_c... Plugins_T>
using plug_in_t =
    util::meta::type_list_concat_t<std::remove_cvref_t<Builder_T>, Builder<Plugins_T...>>;

}   // namespace ddge::app
