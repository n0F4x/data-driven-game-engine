module;

#include <functional>
#include <type_traits>

export module extensions.scheduler.argument_providers.ECSProvider;

import app;

import core.ecs;

import extensions.scheduler.accessors.ecs;

import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::argument_providers {

export template <typename ECSAddon_T>
class ECSProvider {
public:
    template <app::has_addons_c<ECSAddon_T> App_T>
    explicit ECSProvider(App_T& app);

    template <typename Accessor_T>
        requires std::same_as<std::remove_cvref_t<Accessor_T>, accessors::ecs::RegistryRef>
    [[nodiscard]]
    auto provide() const -> std::remove_cvref_t<Accessor_T>;

    template <typename Accessor_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Accessor_T>, accessors::ecs::Query>
        [[nodiscard]]
        auto provide() const -> std::remove_cvref_t<Accessor_T>;

private:
    std::reference_wrapper<core::ecs::Registry> m_registry;
};

}   // namespace extensions::scheduler::argument_providers

template <typename ECSAddon_T>
template <app::has_addons_c<ECSAddon_T> App_T>
extensions::scheduler::argument_providers::ECSProvider<ECSAddon_T>::ECSProvider(App_T& app)
    : m_registry{ app.registry }
{}

template <typename ECSAddon_T>
template <typename Accessor_T>
    requires std::same_as<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::ecs::RegistryRef>
auto extensions::scheduler::argument_providers::ECSProvider<ECSAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
{
    return std::remove_cvref_t<Accessor_T>{ m_registry };
}

template <typename ECSAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::ecs::Query>
auto extensions::scheduler::argument_providers::ECSProvider<ECSAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
{
    return std::remove_cvref_t<Accessor_T>{ m_registry };
}
