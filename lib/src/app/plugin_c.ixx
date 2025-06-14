module;

#include <type_traits>

export module app.plugin_c;

namespace app {

export template <typename T>
concept plugin_c = std::is_class_v<T> && std::is_move_constructible_v<T>;

}   // namespace app
