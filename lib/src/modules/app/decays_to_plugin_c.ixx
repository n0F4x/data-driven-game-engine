module;

#include <type_traits>

export module ddge.modules.app.decays_to_plugin_c;

import ddge.modules.app.plugin_c;

namespace ddge::app {

export template <typename T>
concept decays_to_plugin_c = plugin_c<std::remove_cvref_t<T>>;

}   // namespace ddge::app
