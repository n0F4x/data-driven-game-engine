module;

#include <type_traits>

export module ddge.modules.app.decays_to_addon_c;

import ddge.modules.app.addon_c;

namespace ddge::app {

export template <typename T>
concept decays_to_addon_c = addon_c<std::remove_cvref_t<T>>;

}   // namespace ddge::app
