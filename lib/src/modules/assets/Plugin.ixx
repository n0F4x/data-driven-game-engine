module;

#include <algorithm>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

#include "utility/contracts_macros.hpp"

export module modules.assets.Plugin;

import modules.app;
import modules.assets.Addon;
import modules.assets.Cached;
import modules.assets.loader_c;
import modules.store.Store;

import utility.contracts;
import utility.meta.concepts.specialization_of;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.TypeList;

namespace modules::assets {

template <typename T>
struct IsCachedLoaderDependency
    : std::bool_constant<
          !std::is_rvalue_reference_v<T>
          && util::meta::
              specialization_of_c<std::remove_cvref_t<T>, Cached>> {};

export template <typename T>
concept loader_injection_c =
    loader_c<util::meta::result_of_t<T>>
    && util::meta::
        type_list_all_of_c<util::meta::arguments_of_t<T>, IsCachedLoaderDependency>;

export template <typename T>
concept decays_to_loader_injection_c = loader_injection_c<std::decay_t<T>>;

export class Plugin {
public:
    template <typename Self_T, typename Loader_T>
        requires loader_c<std::remove_cvref_t<Loader_T>>
    auto insert_loader(this Self_T&& self, Loader_T&& loader) -> Self_T;

    template <typename Self_T, decays_to_loader_injection_c Injection_T>
    auto inject_loader(this Self_T&& self, Injection_T&& injection) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> app::add_on_t<App_T, Addon>;

    template <loader_c Loader_T>
    [[nodiscard]]
    auto contains_loader() const -> bool;

private:
    using Caller = std::function<void(store::Store&)>;

    store::Store        m_injections;
    std::vector<Caller>          m_callers;
    std::vector<std::type_index> m_types;
};

}   // namespace modules::assets

template <typename Self_T, typename Loader_T>
    requires modules::assets::loader_c<std::remove_cvref_t<Loader_T>>
auto modules::assets::Plugin::insert_loader(this Self_T&& self, Loader_T&& loader)
    -> Self_T
{
    using Loader = std::remove_cvref_t<Loader_T>;

    Plugin& this_self{ static_cast<Plugin&>(self) };

    PRECOND((!this_self.contains_loader<Loader>()));

    struct Injection {
        Loader loader;
    };

    Injection& injection = this_self.m_injections.emplace<Injection>(Injection{
        std::forward<Loader_T>(loader) });

    this_self.m_callers.push_back([&injection](store::Store& store) -> void {
        store.emplace<Cached<Loader>>(std::move(injection.loader));
    });

    this_self.m_types.push_back(typeid(Loader));

    return std::forward<Self_T>(self);
}

template <typename Injection_T>
auto call_injection(Injection_T&& injection, modules::store::Store& parameter_store)
    -> util::meta::result_of_t<Injection_T>
{
    using Parameters = util::meta::arguments_of_t<Injection_T>;
    static_assert(util::meta::type_list_all_of_c<Parameters, std::is_lvalue_reference>);

    return [&injection,
            &parameter_store]<typename... Parameters_T>(util::TypeList<Parameters_T...>) {
        return std::invoke(
            std::forward<Injection_T>(injection),
            parameter_store.at<std::remove_cvref_t<Parameters_T>>()...
        );
    }(Parameters{});
}

template <typename Self_T, modules::assets::decays_to_loader_injection_c Injection_T>
auto modules::assets::Plugin::inject_loader(this Self_T&& self, Injection_T&& injection)
    -> Self_T
{
    using Loader    = std::remove_cvref_t<util::meta::result_of_t<Injection_T>>;
    using Injection = std::decay_t<Injection_T>;

    Plugin& this_self{ static_cast<Plugin&>(self) };

    PRECOND((!this_self.contains_loader<Loader>()));

    Injection& stored_injection =
        this_self.m_injections.emplace<Injection>(std::forward<Injection_T>(injection));

    this_self.m_callers.push_back(
        [&stored_injection](store::Store& store) -> void {
            store.emplace<Loader>(::call_injection(std::move(stored_injection), store));
        }
    );

    this_self.m_types.push_back(typeid(Loader));

    return std::forward<Self_T>(self);
}

template <modules::app::decays_to_app_c App_T>
auto modules::assets::Plugin::build(
    App_T&& app
) && -> app::add_on_t<App_T, Addon>
{
    static_assert(!app::has_addons_c<App_T, Addon>);

    store::Store store;
    for (const Caller& caller : m_callers) {
        caller(store);
    }

    return std::forward<App_T>(app).add_on(
        Addon{
            .asset_manager{ std::move(store) },
        }
    );
}

template <modules::assets::loader_c Loader_T>
auto modules::assets::Plugin::contains_loader() const -> bool
{
    return std::ranges::contains(m_types, typeid(Loader_T));
}
