module;

#include <type_traits>

export module modules.app.decays_to_plugin_c;

import modules.app.plugin_c;

namespace modules::app {

export template <typename T>
concept decays_to_plugin_c = plugin_c<std::remove_cvref_t<T>>;

}   // namespace app
