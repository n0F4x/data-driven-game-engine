module;

#include <functional>
#include <type_traits>

#include "utility/contract_macros.hpp"

export module ddge.modules.scheduler.accessors.assets:Provider;

import :Cached;

import ddge.modules.scheduler.ProviderFor;
import ddge.modules.scheduler.ProviderOf;

import ddge.modules.app;
import ddge.modules.assets;

import ddge.utility.contracts;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.specialization_of;

namespace ddge::scheduler::accessors {

inline namespace assets {

export class Provider {
public:
    template <ddge::app::has_addons_c<ddge::assets::Addon> App_T>
    explicit Provider(App_T& app);

    template <util::meta::specialization_of_c<accessors::assets::Cached> Cached_T>
    [[nodiscard]]
    auto provide() const -> Cached_T;

private:
    std::reference_wrapper<ddge::assets::AssetManager> m_asset_manager_ref;
};

}   // namespace assets

}   // namespace ddge::scheduler::accessors

template <>
struct ddge::scheduler::ProviderFor<ddge::assets::Addon>
    : std::type_identity<ddge::scheduler::accessors::assets::Provider> {};

template <typename Loader_T>
struct ddge::scheduler::ProviderOf<ddge::scheduler::accessors::assets::Cached<Loader_T>>
    : std::type_identity<ddge::scheduler::accessors::assets::Provider> {};

template <ddge::app::has_addons_c<ddge::assets::Addon> App_T>
ddge::scheduler::accessors::assets::Provider::Provider(App_T& app)
    : m_asset_manager_ref{ app.asset_manager }
{}

template <ddge::util::meta::
              specialization_of_c<ddge::scheduler::accessors::assets::Cached> Cached_T>
auto ddge::scheduler::accessors::assets::Provider::provide() const -> Cached_T
{
    using Loader = typename Cached_T::Loader;

    PRECOND(m_asset_manager_ref.get().contains<Loader>());

    return Cached_T{ m_asset_manager_ref.get().at<Loader>() };
}
