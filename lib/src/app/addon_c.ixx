module;

#include <type_traits>

export module app.addon_c;

namespace app {

export template <typename T>
concept addon_c = std::is_class_v<T> && std::is_move_constructible_v<T>;

}   // namespace app
