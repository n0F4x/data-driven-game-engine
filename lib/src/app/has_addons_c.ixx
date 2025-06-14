module;

#include <concepts>

export module app.has_addons_c;

import app.app_c;

namespace app {

export template <typename App_T, typename... Addons_T>
concept has_addons_c = app_c<std::remove_cvref_t<App_T>>
                    && (std::derived_from<std::remove_cvref_t<App_T>, Addons_T> && ...);

}   // namespace app
