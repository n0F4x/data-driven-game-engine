module;

#include <algorithm>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

#include "utility/contracts_macros.hpp"

export module plugins.resources;

import addons.Resources;

import app;

import modules.resources;
import modules.store.Store;

import utility.contracts;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.TypeList;

namespace plugins {

export template <typename T>
concept resource_c = modules::resources::resource_c<T>;

template <typename T>
struct IsResourceDependency
    : std::bool_constant<
          !std::is_rvalue_reference_v<T> && resource_c<std::remove_cvref_t<T>>> {};

export template <typename T>
concept resource_injection_c =
    resource_c<util::meta::result_of_t<T>>
    && util::meta::type_list_all_of_c<util::meta::arguments_of_t<T>, IsResourceDependency>;

export template <typename T>
concept decays_to_resource_injection_c = resource_injection_c<std::decay_t<T>>;

export class Resources {
public:
    template <typename Self_T, typename Resource_T>
        requires plugins::resource_c<std::remove_cvref_t<Resource_T>>
    auto insert_resource(this Self_T&& self, Resource_T&& resource) -> Self_T;

    template <typename Self_T, plugins::decays_to_resource_injection_c Injection_T>
    auto inject_resource(this Self_T&& self, Injection_T&& injection) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> app::add_on_t<App_T, addons::Resources>;

    template <resource_c Resource_T>
    [[nodiscard]]
    auto contains_resource() const -> bool;

private:
    using Caller = std::function<void(modules::store::Store&)>;

    modules::store::Store           m_injections;
    std::vector<Caller>          m_callers;
    std::vector<std::type_index> m_types;
};

}   // namespace plugins

template <typename Self_T, typename Resource_T>
    requires plugins::resource_c<std::remove_cvref_t<Resource_T>>
auto plugins::Resources::insert_resource(this Self_T&& self, Resource_T&& resource)
    -> Self_T
{
    using Resource = std::remove_cvref_t<Resource_T>;

    Resources& this_self{ static_cast<Resources&>(self) };

    PRECOND((!this_self.contains_resource<Resource>()));

    struct Injection {
        Resource resource;
    };

    Injection& injection = this_self.m_injections.emplace<Injection>(Injection{
        std::forward<Resource_T>(resource) });

    this_self.m_callers.push_back([&injection](modules::store::Store& store) -> void {
        store.emplace<Resource>(std::move(injection.resource));
    });

    this_self.m_types.push_back(typeid(Resource));

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

template <typename Self_T, plugins::decays_to_resource_injection_c Injection_T>
auto plugins::Resources::inject_resource(this Self_T&& self, Injection_T&& injection)
    -> Self_T
{
    using Resource  = std::remove_cvref_t<util::meta::result_of_t<Injection_T>>;
    using Injection = std::decay_t<Injection_T>;

    Resources& this_self{ static_cast<Resources&>(self) };

    PRECOND((!this_self.contains_resource<Resource>()));

    if constexpr (requires { requires std::is_function_v<decltype(Injection_T::setup)>; })
    {
        ::call_injection(Injection_T::setup, this_self.m_injections);
    }
    else if constexpr (requires {
                           requires std::
                               is_member_function_pointer_v<decltype(&Injection_T::setup)>;
                       })
    {
        ::call_injection(
            std::bind_front(&Injection_T::setup, injection), this_self.m_injections
        );
    }

    Injection& stored_injection =
        this_self.m_injections.emplace<Injection>(std::forward<Injection_T>(injection));

    this_self.m_callers.push_back([&stored_injection](modules::store::Store& store) -> void {
        store.emplace<Resource>(::call_injection(std::move(stored_injection), store));
    });

    this_self.m_types.push_back(typeid(Resource));

    return std::forward<Self_T>(self);
}

template <app::decays_to_app_c App_T>
auto plugins::Resources::build(App_T&& app) && -> app::add_on_t<App_T, addons::Resources>
{
    static_assert(!app::has_addons_c<App_T, addons::Resources>);

    modules::store::Store store;
    for (const Caller& caller : m_callers) {
        caller(store);
    }

    return std::forward<App_T>(app).add_on(
        addons::Resources{
            .resource_manager{ std::move(store) },
        }
    );
}

template <plugins::resource_c Resource_T>
auto plugins::Resources::contains_resource() const -> bool
{
    return std::ranges::contains(m_types, typeid(Resource_T));
}
