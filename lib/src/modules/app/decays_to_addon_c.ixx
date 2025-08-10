module;

#include <type_traits>

export module modules.app.decays_to_addon_c;

import modules.app.addon_c;

namespace modules::app {

export template <typename T>
concept decays_to_addon_c = addon_c<std::remove_cvref_t<T>>;

}   // namespace app
