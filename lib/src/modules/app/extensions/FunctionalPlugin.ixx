module;

#include <functional>

export module ddge.modules.app.extensions.FunctionalPlugin;

import ddge.modules.app.decays_to_builder_c;

namespace ddge::app {

inline namespace extensions {

template <typename Modifier_T, typename Builder_T>
concept modifier_c = requires {
    std::derived_from<
        std::remove_cvref_t<std::invoke_result_t<Modifier_T&&, Builder_T&&>>,
        Builder_T>;
};

export class FunctionalPlugin {
public:
    template <decays_to_builder_c Self_T, modifier_c<Self_T> Modifier_T>
    constexpr auto transform(this Self_T&&, Modifier_T&& modifier)
        -> std::invoke_result_t<Modifier_T, Self_T&&>;
};

}   // namespace extensions

}   // namespace ddge::app

template <
    ddge::app::decays_to_builder_c            Self_T,
    ddge::app::extensions::modifier_c<Self_T> Modifier_T>
constexpr auto ddge::app::extensions::FunctionalPlugin::transform(
    this Self_T&& self,
    Modifier_T&&  modifier
) -> std::invoke_result_t<Modifier_T, Self_T&&>
{
    return std::invoke(std::forward<Modifier_T>(modifier), std::forward<Self_T>(self));
}
