module;

#include <type_traits>

export module modules.app.decays_to_app_c;

import modules.app.app_c;

namespace modules::app {

export template <typename T>
concept decays_to_app_c = app_c<std::remove_cvref_t<T>>;

}   // namespace app
