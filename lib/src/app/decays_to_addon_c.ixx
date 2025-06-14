module;

#include <type_traits>

export module app.decays_to_addon_c;

import app.addon_c;

namespace app {

export template <typename T>
concept decays_to_addon_c = addon_c<std::remove_cvref_t<T>>;

}   // namespace app
