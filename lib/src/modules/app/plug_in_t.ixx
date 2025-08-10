module;

#include <utility>

export module modules.app.plug_in_t;

import modules.app.Builder;
import modules.app.decays_to_builder_c;
import modules.app.plugin_c;

import utility.meta.type_traits.type_list.type_list_concat;

namespace modules::app {

export template <decays_to_builder_c Builder_T, plugin_c... Plugins_T>
using plug_in_t =
    util::meta::type_list_concat_t<std::remove_cvref_t<Builder_T>, Builder<Plugins_T...>>;

}   // namespace app
