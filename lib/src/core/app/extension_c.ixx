module;

#include <type_traits>

export module core.app.extension_c;

namespace core::app {

export template <typename T>
concept extension_c = std::is_class_v<T> && std::is_move_constructible_v<T>;

}   // namespace core::app
