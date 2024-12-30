module;

#include <type_traits>

export module core.app.Builder:customization_c;

export template <typename Customization_T>
concept customization_c = std::is_class_v<Customization_T>
                       && std::is_move_constructible_v<Customization_T>;
