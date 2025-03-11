module;

#include <type_traits>

export module core.app.Builder:extension_c;

export template <typename Extension_T>
concept extension_c = std::is_class_v<Extension_T>
                       && std::is_move_constructible_v<Extension_T>;
