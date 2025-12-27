module;

#include <algorithm>
#include <concepts>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

#include "utility/contract_macros.hpp"

export module ddge.modules.resources.Plugin;

import ddge.modules.app;

import ddge.modules.resources.Addon;
import ddge.modules.resources.resource_c;
import ddge.utility.containers.store.Store;

import ddge.utility.contracts;
import ddge.utility.meta.concepts.type_list.type_list_all_of;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.meta.type_traits.type_list.type_list_single;
import ddge.utility.TypeList;

namespace ddge::resources {

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

export class Plugin {
public:
    template <typename Self_T, typename Resource_T>
        requires resource_c<std::remove_cvref_t<Resource_T>>
    auto insert_resource(this Self_T&&, Resource_T&& resource) -> Self_T;

    template <typename Self_T, decays_to_resource_injection_c Injection_T>
    auto inject_resource(
        this Self_T&&,
        Injection_T&&               injection,
        std::decay_t<Injection_T>** out = nullptr
    ) -> Self_T;

    template <resource_injection_c Injection_T, typename Self_T, typename UInjection_T>
        requires std::convertible_to<UInjection_T&&, Injection_T>
    auto try_inject_resource(
        this Self_T&&,
        UInjection_T&& injection,
        Injection_T**  out = nullptr
    ) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> app::add_on_t<App_T, Addon>;

private:
    using Caller = std::function<void(util::store::Store&)>;

    util::store::Store           m_injections;
    std::vector<Caller>          m_callers;
    std::vector<std::type_index> m_types;

    template <resource_c Resource_T>
    [[nodiscard]]
    auto contains_resource() const -> bool;
};

}   // namespace ddge::resources

namespace ddge::resources {

template <typename Self_T, typename Resource_T>
    requires resource_c<std::remove_cvref_t<Resource_T>>
auto Plugin::insert_resource(this Self_T&& self, Resource_T&& resource) -> Self_T
{
    using Resource = std::remove_cvref_t<Resource_T>;

    Plugin& this_self{ static_cast<Plugin&>(self) };

    PRECOND((!this_self.contains_resource<Resource>()));

    struct Injection {
        Resource resource;
    };

    Injection& injection = this_self.m_injections.emplace<Injection>(
        Injection{ .resource = std::forward<Resource_T>(resource) }   //
    );

    this_self.m_callers.push_back([&injection](util::store::Store& store) -> void {
        store.emplace<Resource>(std::move(injection.resource));
    });

    this_self.m_types.push_back(typeid(Resource));

    return std::forward<Self_T>(self);
}

template <typename Injection_T>
auto apply_injections(Injection_T&& injection, ddge::util::store::Store& parameter_store)
    -> ddge::util::meta::result_of_t<Injection_T>
{
    using Parameters = ddge::util::meta::arguments_of_t<Injection_T>;
    static_assert(
        ddge::util::meta::type_list_all_of_c<Parameters, std::is_lvalue_reference>
    );

    return [&injection, &parameter_store]<typename... Parameters_T>(
               ddge::util::TypeList<Parameters_T...>
           ) {
        PRECOND((parameter_store.contains<Parameters_T>() && ...));

        return std::invoke(
            std::forward<Injection_T>(injection),
            parameter_store.at<std::remove_cvref_t<Parameters_T>>()...
        );
    }(Parameters{});
}

template <typename Self_T, decays_to_resource_injection_c Injection_T>
auto Plugin::inject_resource(
    this Self_T&&               self,
    Injection_T&&               injection,
    std::decay_t<Injection_T>** out
) -> Self_T
{
    using Resource  = std::remove_cvref_t<util::meta::result_of_t<Injection_T>>;
    using Injection = std::decay_t<Injection_T>;

    Plugin& this_self{ static_cast<Plugin&>(self) };

    PRECOND((!this_self.contains_resource<Resource>()));

    if constexpr (requires { requires std::is_function_v<decltype(Injection_T::setup)>; })
    {
        apply_injections(Injection_T::setup, this_self.m_injections);
    }
    else if constexpr (requires {
                           requires std::
                               is_member_function_pointer_v<decltype(&Injection_T::setup)>;
                       })
    {
        apply_injections(
            std::bind_front(&Injection_T::setup, injection), this_self.m_injections
        );
    }

    Injection& stored_injection =
        this_self.m_injections.emplace<Injection>(std::forward<Injection_T>(injection));

    this_self.m_callers.push_back([&stored_injection](util::store::Store& store) -> void {
        store.emplace<Resource>(apply_injections(std::move(stored_injection), store));
    });

    this_self.m_types.push_back(typeid(Resource));

    if (out != nullptr) {
        *out = std::addressof(stored_injection);
    }

    return std::forward<Self_T>(self);
}

template <resource_injection_c Injection_T, typename Self_T, typename UInjection_T>
    requires std::convertible_to<UInjection_T&&, Injection_T>
auto Plugin::try_inject_resource(
    this Self_T&&  self,
    UInjection_T&& injection,
    Injection_T**  out
) -> Self_T
{
    using Resource = std::remove_cvref_t<util::meta::result_of_t<Injection_T>>;
    return !self.Plugin::template contains_resource<Resource>()
             ? std::forward<Self_T>(self).inject_resource(
                   static_cast<Injection_T>(std::forward<UInjection_T>(injection)), out
               )
             : std::forward<Self_T>(self);
}

template <ddge::app::decays_to_app_c App_T>
auto Plugin::build(App_T&& app) && -> app::add_on_t<App_T, Addon>
{
    static_assert(!app::has_addons_c<App_T, Addon>);

    util::store::Store store;
    for (const Caller& caller : m_callers) {
        caller(store);
    }

    return std::forward<App_T>(app).add_on(
        Addon{
            .resource_manager{ std::move(store) },
        }
    );
}

template <resource_c Resource_T>
auto Plugin::contains_resource() const -> bool
{
    return std::ranges::contains(m_types, typeid(Resource_T));
}

}   // namespace ddge::resources
