module;

#include <concepts>

export module app.has_plugins_c;

import app.builder_c;

namespace app {

export template <typename Builder_T, typename... Extensions_T>
concept has_plugins_c = builder_c<std::remove_cvref_t<Builder_T>>
                       && (std::derived_from<std::remove_cvref_t<Builder_T>, Extensions_T>
                           && ...);

}   // namespace app
