module;

#include <type_traits>

export module core.app.App:addon_c;

template <typename Addon_T>
concept addon_c = std::is_class_v<Addon_T> && std::is_move_constructible_v<Addon_T>;
