module;

#include <type_traits>

export module core.app.App:mixin_c;

template <typename Mixin_T>
concept mixin_c = std::is_class_v<Mixin_T> && std::is_move_constructible_v<Mixin_T>;
