module;

#include <concepts>

export module ddge.modules.app.has_plugins_c;

import ddge.modules.app.builder_c;

namespace ddge::app {

export template <typename Builder_T, typename... Extensions_T>
concept has_plugins_c = builder_c<std::remove_cvref_t<Builder_T>>
                     && (std::derived_from<std::remove_cvref_t<Builder_T>, Extensions_T>
                         && ...);

}   // namespace ddge::app
