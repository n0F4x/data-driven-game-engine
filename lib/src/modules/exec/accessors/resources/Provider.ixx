module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.accessors.resources:Provider;

import :Resource;

import ddge.modules.app;
import ddge.modules.exec.ProviderFor;
import ddge.modules.exec.ProviderOf;
import ddge.modules.resources;

import ddge.utility.contracts;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::exec::accessors {

inline namespace resources {

export class Provider {
public:
    template <ddge::app::has_addons_c<ddge::resources::Addon> App_T>
    constexpr explicit Provider(App_T& app);

    template <util::meta::specialization_of_c<accessors::resources::Resource> Resource_T>
    [[nodiscard]]
    constexpr auto provide() const -> Resource_T;

private:
    std::reference_wrapper<ddge::resources::ResourceManager> m_resource_manager_ref;
};

}   // namespace resources

}   // namespace ddge::exec::accessors

template <>
struct ddge::exec::ProviderFor<ddge::resources::Addon>
    : std::type_identity<ddge::exec::accessors::resources::Provider> {};

template <typename Resource_T>
struct ddge::exec::ProviderOf<ddge::exec::accessors::resources::Resource<Resource_T>>
    : std::type_identity<ddge::exec::accessors::resources::Provider> {};

template <ddge::app::has_addons_c<ddge::resources::Addon> App_T>
constexpr ddge::exec::accessors::resources::Provider::Provider(App_T& app)
    : m_resource_manager_ref{ app.resource_manager }
{}

template <ddge::util::meta::
              specialization_of_c<ddge::exec::accessors::resources::Resource> Resource_T>
constexpr auto ddge::exec::accessors::resources::Provider::provide() const -> Resource_T
{
    using Resource = std::remove_const_t<typename Resource_T::Underlying>;

    PRECOND(
        (m_resource_manager_ref.get().contains<Resource>()),
        std::format("Resource `{}` does not exist", util::meta::name_of<Resource>())
    );

    return Resource_T{ m_resource_manager_ref.get().at<Resource>() };
}
