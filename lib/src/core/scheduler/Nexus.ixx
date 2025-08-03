module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module core.scheduler.Nexus;

import core.scheduler.accessor_c;
import core.scheduler.ProviderFor;

import core.store;

import utility.contracts;
import utility.meta.algorithms.for_each;
import utility.meta.reflection.name_of;
import utility.meta.concepts.all_different;

namespace core::scheduler {

export class Nexus {
public:
    template <typename... Providers_T>
        requires util::meta::all_different_c<std::remove_cvref_t<Providers_T>...>
    explicit Nexus(Providers_T&&... providers);

    template <accessor_c Accessor_T>
    [[nodiscard]]
    auto provide() -> Accessor_T;

private:
    template <typename Accessor_T>
    using Producer = std::function<Accessor_T()>;

    store::Store m_providers;
};

}   // namespace core::scheduler

template <typename... Providers_T>
    requires util::meta::all_different_c<std::remove_cvref_t<Providers_T>...>
core::scheduler::Nexus::Nexus(Providers_T&&... providers)
{
    (m_providers.emplace<std::remove_cvref_t<Providers_T>>(
         std::forward<Providers_T>(providers)
     ),
     ...);
}

template <core::scheduler::accessor_c Accessor_T>
auto core::scheduler::Nexus::provide() -> Accessor_T
{
    using Provider = provider_for_t<Accessor_T>;

    PRECOND(
        m_providers.contains<Provider>(),
        std::format(
            "Accessor {} (that has provider {}) is not supported",
            util::meta::name_of<Accessor_T>(),
            util::meta::name_of<Provider>()
        )
    );

    return m_providers.at<Provider>().template provide<Accessor_T>();
}
