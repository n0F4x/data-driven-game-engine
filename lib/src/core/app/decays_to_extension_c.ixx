module;

#include <type_traits>

export module core.app.decays_to_extension_c;

import core.app.extension_c;

namespace core::app {

export template <typename T>
concept decays_to_extension_c = extension_c<std::remove_cvref_t<T>>;

}   // namespace core::app
