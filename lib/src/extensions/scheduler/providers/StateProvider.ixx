module;

#include <concepts>
#include <functional>
#include <type_traits>

export module extensions.scheduler.providers.StateProvider;

import app;

import core.states.StateManager;

import extensions.scheduler.accessors.states;

import utility.meta.concepts.decays_to;
import utility.meta.concepts.specialization_of;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::providers {

export template <typename StatesAddon_T>
class StateProvider {
    using StateManager =
        std::remove_reference_t<decltype(std::declval<StatesAddon_T>().state_manager)>;

public:
    template <app::has_addons_c<StatesAddon_T> App_T>
    constexpr explicit StateProvider(App_T& app);

    template <typename Accessor_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Accessor_T>, accessors::states::State>
        [[nodiscard]]
        constexpr auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(StateManager::template registered<std::remove_const_t<
                     util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>());

private:
    std::reference_wrapper<StateManager> m_state_manager_ref;
};

}   // namespace extensions::scheduler::providers

template <typename StatesAddon_T>
template <app::has_addons_c<StatesAddon_T> App_T>
constexpr extensions::scheduler::providers::StateProvider<StatesAddon_T>::StateProvider(
    App_T& app
)
    : m_state_manager_ref{ app.state_manager }
{}

template <typename StatesAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::states::State>
constexpr auto extensions::scheduler::providers::StateProvider<StatesAddon_T>::provide(
) const -> std::remove_cvref_t<Accessor_T>
    requires(StateManager::template registered<std::remove_const_t<
                 util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>())
{
    return std::remove_cvref_t<Accessor_T>{
        m_state_manager_ref.get()
            .template get<std::remove_const_t<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>()
    };
}
