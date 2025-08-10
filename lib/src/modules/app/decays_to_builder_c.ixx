module;

#include <type_traits>

export module modules.app.decays_to_builder_c;

import modules.app.builder_c;

namespace modules::app {

export template <typename T>
concept decays_to_builder_c = builder_c<std::remove_cvref_t<T>>;

}   // namespace app
