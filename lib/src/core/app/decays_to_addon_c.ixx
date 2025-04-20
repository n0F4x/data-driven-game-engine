module;

#include <type_traits>

export module core.app.decays_to_addon_c;

import core.app.addon_c;

namespace core::app {

export template <typename T>
concept decays_to_addon_c = addon_c<std::remove_cvref_t<T>>;

}   // namespace core::app
