module;

#include <functional>

export module addons.functional.Customization;

import core.app.Builder;

namespace plugins::functional {

template <typename Modifier_T, typename Builder_T>
concept modifier_c = requires {
    std::derived_from<
        std::remove_cvref_t<std::invoke_result_t<Modifier_T&&, Builder_T&&>>,
        Builder_T>;
};

export class Customization {
public:
    template <core::app::builder_c Self_T, modifier_c<Self_T> Modifier_T>
    auto apply(this Self_T&&, Modifier_T&& modifier)
        -> std::invoke_result_t<Modifier_T&&, Self_T&&>;
};

}   // namespace plugins::functional

template <core::app::builder_c Self_T, plugins::functional::modifier_c<Self_T> Modifier_T>
auto plugins::functional::Customization::apply(this Self_T&& self, Modifier_T&& modifier)
    -> std::invoke_result_t<Modifier_T&&, Self_T&&>
{
    return std::invoke(std::forward<Modifier_T>(modifier), std::forward<Self_T>(self));
}
