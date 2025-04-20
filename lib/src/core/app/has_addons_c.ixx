module;

#include <concepts>

export module core.app.has_addons_c;

import core.app.app_c;

namespace core::app {

export template <typename App_T, typename... Addons_T>
concept has_addons_c = app_c<std::remove_cvref_t<App_T>>
                    && (std::derived_from<std::remove_cvref_t<App_T>, Addons_T> && ...);

}   // namespace core::app
