module;

#include <type_traits>

export module core.app.decays_to_app_c;

import core.app.app_c;

namespace core::app {

export template <typename T>
concept decays_to_app_c = app_c<std::remove_cvref_t<T>>;

}   // namespace core::app
