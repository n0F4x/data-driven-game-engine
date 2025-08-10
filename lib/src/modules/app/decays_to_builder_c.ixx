module;

#include <type_traits>

export module ddge.modules.app.decays_to_builder_c;

import ddge.modules.app.builder_c;

namespace ddge::app {

export template <typename T>
concept decays_to_builder_c = builder_c<std::remove_cvref_t<T>>;

}   // namespace ddge::app
