module;

#include <utility>

export module ddge.modules.app.add_on_t;

import ddge.modules.app.addon_c;
import ddge.modules.app.App;
import ddge.modules.app.decays_to_app_c;

import ddge.utility.meta.type_traits.type_list.type_list_concat;

namespace ddge::app {

export template <decays_to_app_c App_T, addon_c... Addons_T>
using add_on_t =
    util::meta::type_list_concat_t<std::remove_cvref_t<App_T>, App<Addons_T...>>;

}   // namespace ddge::app
