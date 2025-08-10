module;

#include <type_traits>

export module ddge.modules.app.plugin_c;

namespace ddge::app {

export template <typename T>
concept plugin_c = std::is_class_v<T> && std::is_move_constructible_v<T>;

}   // namespace ddge::app
