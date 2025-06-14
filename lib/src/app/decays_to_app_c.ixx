module;

#include <type_traits>

export module app.decays_to_app_c;

import app.app_c;

namespace app {

export template <typename T>
concept decays_to_app_c = app_c<std::remove_cvref_t<T>>;

}   // namespace app
