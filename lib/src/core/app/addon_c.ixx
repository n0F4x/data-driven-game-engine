module;

#include <type_traits>

export module core.app.addon_c;

namespace core::app {

export template <typename T>
concept addon_c = std::is_class_v<T> && std::is_move_constructible_v<T>;

}   // namespace core::app
