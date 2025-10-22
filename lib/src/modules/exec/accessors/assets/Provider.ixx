module;

#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.accessors.assets:Provider;

import :Cached;

import ddge.modules.exec.ProviderFor;
import ddge.modules.exec.ProviderOf;

import ddge.modules.app;
import ddge.modules.assets;

import ddge.utility.contracts;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.specialization_of;

namespace ddge::exec::accessors {

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

}   // namespace ddge::exec::accessors

template <>
struct ddge::exec::ProviderFor<ddge::assets::Addon>
    : std::type_identity<ddge::exec::accessors::assets::Provider> {};

template <typename Loader_T>
struct ddge::exec::ProviderOf<ddge::exec::accessors::assets::Cached<Loader_T>>
    : std::type_identity<ddge::exec::accessors::assets::Provider> {};

template <ddge::app::has_addons_c<ddge::assets::Addon> App_T>
ddge::exec::accessors::assets::Provider::Provider(App_T& app)
    : m_asset_manager_ref{ app.asset_manager }
{}

template <
    ddge::util::meta::specialization_of_c<ddge::exec::accessors::assets::Cached> Cached_T>
auto ddge::exec::accessors::assets::Provider::provide() const -> Cached_T
{
    using Loader = typename Cached_T::Loader;

    PRECOND(m_asset_manager_ref.get().contains<Loader>());

    return Cached_T{ m_asset_manager_ref.get().at<Loader>() };
}
