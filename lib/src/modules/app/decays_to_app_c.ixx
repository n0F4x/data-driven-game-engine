module;

#include <type_traits>

export module ddge.modules.app.decays_to_app_c;

import ddge.modules.app.app_c;

namespace ddge::app {

export template <typename T>
concept decays_to_app_c = app_c<std::remove_cvref_t<T>>;

}   // namespace ddge::app
