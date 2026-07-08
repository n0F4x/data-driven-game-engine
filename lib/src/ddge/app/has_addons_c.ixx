module;

#include <concepts>

export module ddge.app.has_addons_c;

import ddge.app.app_c;

namespace ddge::app {

export template <typename App_T, typename... Addons_T>
concept has_addons_c = app_c<std::remove_cvref_t<App_T>>
                    && (std::derived_from<std::remove_cvref_t<App_T>, Addons_T> && ...);

}   // namespace ddge::app
