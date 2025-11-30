module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contract_macros.hpp"

export module ddge.modules.scheduler.Nexus;

import ddge.modules.scheduler.accessor_c;
import ddge.modules.scheduler.ProviderFor;

import ddge.utility.containers.store;

import ddge.utility.contracts;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.concepts.all_different;

namespace ddge::scheduler {

export class Nexus {
public:
    template <typename... Providers_T>
        requires util::meta::all_different_c<std::remove_cvref_t<Providers_T>...>
    explicit Nexus(Providers_T&&... providers);

    template <accessor_c Accessor_T>
    [[nodiscard]]
    auto provide() -> Accessor_T;

private:
    utility::store::Store m_providers;
};

}   // namespace ddge::scheduler

template <typename... Providers_T>
    requires ddge::util::meta::all_different_c<std::remove_cvref_t<Providers_T>...>
ddge::scheduler::Nexus::Nexus(Providers_T&&... providers)
{
    (m_providers.emplace<std::remove_cvref_t<Providers_T>>(
         std::forward<Providers_T>(providers)
     ),
     ...);
}

template <ddge::scheduler::accessor_c Accessor_T>
auto ddge::scheduler::Nexus::provide() -> Accessor_T
{
    using Provider = provider_of_t<Accessor_T>;

    PRECOND(
        m_providers.contains<Provider>(),
        std::format(
            "Provider `{}` of accessor `{}` is not found",
            util::meta::name_of<Provider>(),
            util::meta::name_of<Accessor_T>()
        )
    );

    return m_providers.at<Provider>().template provide<Accessor_T>();
}
