module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.Nexus;

import ddge.modules.exec.accessor_c;
import ddge.modules.exec.ProviderFor;

import ddge.modules.store;

import ddge.utility.contracts;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.concepts.all_different;

namespace ddge::exec {

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

}   // namespace ddge::exec

template <typename... Providers_T>
    requires ddge::util::meta::all_different_c<std::remove_cvref_t<Providers_T>...>
ddge::exec::Nexus::Nexus(Providers_T&&... providers)
{
    (m_providers.emplace<std::remove_cvref_t<Providers_T>>(
         std::forward<Providers_T>(providers)
     ),
     ...);
}

template <ddge::exec::accessor_c Accessor_T>
auto ddge::exec::Nexus::provide() -> Accessor_T
{
    using Provider = provider_for_t<Accessor_T>;

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
