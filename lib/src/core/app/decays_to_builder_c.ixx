module;

#include <type_traits>

export module core.app.decays_to_builder_c;

import core.app.builder_c;

namespace core::app {

export template <typename T>
concept decays_to_builder_c = builder_c<std::remove_cvref_t<T>>;

}   // namespace core::app
