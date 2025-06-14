module;

#include <type_traits>

export module app.decays_to_builder_c;

import app.builder_c;

namespace app {

export template <typename T>
concept decays_to_builder_c = builder_c<std::remove_cvref_t<T>>;

}   // namespace app
