module;

#include <functional>

export module extensions.Functional;

import core.app.decays_to_builder_c;

namespace extensions {

template <typename Modifier_T, typename Builder_T>
concept modifier_c = requires {
    std::derived_from<
        std::remove_cvref_t<std::invoke_result_t<Modifier_T&&, Builder_T&&>>,
        Builder_T>;
};

export class Functional {
public:
    template <core::app::decays_to_builder_c Self_T, modifier_c<Self_T> Modifier_T>
    constexpr auto transform(this Self_T&&, Modifier_T&& modifier)
        -> std::invoke_result_t<Modifier_T, Self_T&&>;
};

}   // namespace extensions

template <core::app::decays_to_builder_c Self_T, extensions::modifier_c<Self_T> Modifier_T>
constexpr auto extensions::Functional::transform(this Self_T&& self, Modifier_T&& modifier)
    -> std::invoke_result_t<Modifier_T, Self_T&&>
{
    return std::invoke(std::forward<Modifier_T>(modifier), std::forward<Self_T>(self));
}
