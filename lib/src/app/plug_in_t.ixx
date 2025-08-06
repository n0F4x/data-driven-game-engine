module;

#include <utility>

export module app.plug_in_t;

import app.Builder;
import app.decays_to_builder_c;
import app.plugin_c;

import utility.meta.type_traits.type_list.type_list_concat;

namespace app {

export template <decays_to_builder_c Builder_T, plugin_c... Plugins_T>
using plug_in_t =
    util::meta::type_list_concat_t<std::remove_cvref_t<Builder_T>, Builder<Plugins_T...>>;

}   // namespace app
