module;

#include <type_traits>

export module app.decays_to_plugin_c;

import app.plugin_c;

namespace app {

export template <typename T>
concept decays_to_plugin_c = plugin_c<std::remove_cvref_t<T>>;

}   // namespace app
