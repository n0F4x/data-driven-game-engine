module;

#include <concepts>

export module core.app.extended_with_c;

import core.app.builder_c;

namespace core::app {

export template <typename Builder_T, typename... Extensions_T>
concept extended_with_c = builder_c<std::remove_cvref_t<Builder_T>>
                       && (std::derived_from<std::remove_cvref_t<Builder_T>, Extensions_T>
                           && ...);

}   // namespace core::app
