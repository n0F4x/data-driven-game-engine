module;

#include <algorithm>
#include <any>
#include <functional>
#include <type_traits>

#include "core/log/log.hpp"

export module extensions.ResourceManager;

import core.app.App;
import core.app.Builder;
import core.store.Store;

import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.invoke_result_of;

import utility.tuple;

import addons.ResourceManager;

namespace extensions {

export template <typename Resource_T>
concept resource_c = addons::resource_c<Resource_T>;

export template <typename Injection_T>
concept injection_c = resource_c<
    util::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Injection_T>>>>;

using InjectionInvocation =
    std::function<void(core::store::Store& injections, core::store::Store& resources)>;

template <typename Injection_T>
[[nodiscard]]
auto create_injection_invocation() -> InjectionInvocation;

export class ResourceManager {
public:
    template <typename Self_T, typename Resource_T>
    auto use_resource(this Self_T&&, Resource_T&& resource) -> Self_T;

    template <typename Self_T, injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection) -> Self_T;

protected:
    template <core::app::app_c App_T>
    auto operator()(App_T&& app) &&;

private:
    core::store::Store               m_injections;
    std::vector<InjectionInvocation> m_invocations;
};

}   // namespace extensions

template <typename Callable_T>
auto gather_parameters(core::store::Store& store)
{
    using RequiredResourcesTuple = util::meta::arguments_of_t<Callable_T>;
    return util::generate_tuple_from<RequiredResourcesTuple>(
        [&store]<typename Resource>() -> Resource {
            return store.at<std::remove_cvref_t<Resource>>();
        }
    );
}

template <typename Injection_T>
auto extensions::create_injection_invocation() -> InjectionInvocation
{
    return [](core::store::Store& injections, core::store::Store& resources) {
        using Injection = std::decay_t<Injection_T>;
        using ResourceType =
            util::meta::invoke_result_of_t<std::remove_pointer_t<Injection>>;

        if (!resources.contains<ResourceType>()) {
            resources.emplace<ResourceType>(std::apply(
                std::ref(injections.at<Injection>()),
                ::gather_parameters<std::remove_pointer_t<Injection>>(resources)
            ));
        }
    };
}

template <typename Self_T, typename Resource_T>
auto extensions::ResourceManager::use_resource(this Self_T&& self, Resource_T&& resource)
    -> Self_T
{
    using Resource = std::remove_cvref_t<Resource_T>;

    struct Injection {
        auto operator()() -> Resource
        {
            return std::move(resource);
        }

        Resource resource;
    };

    return std::forward<Self_T>(self).inject_resource(
        Injection{ std::forward<Resource_T>(resource) }
    );
}

template <typename Self_T, extensions::injection_c Injection_T>
auto extensions::ResourceManager::inject_resource(
    this Self_T&& self,
    Injection_T&& injection
) -> Self_T
{
    if constexpr (requires { requires std::is_function_v<decltype(Injection_T::setup)>; })
    {
        std::apply(
            Injection_T::setup,
            ::gather_parameters<decltype(Injection_T::setup)>(self.m_injections)
        );
    }
    else if constexpr (requires {
                           requires std::
                               is_member_function_pointer_v<decltype(&Injection_T::setup)>;
                       })
    {
        std::apply(
            std::bind_front(&Injection_T::setup, injection),
            ::gather_parameters<decltype(&Injection_T::setup)>(self.m_injections)
        );
    }

    self.m_injections.template emplace<std::decay_t<Injection_T>>(
        std::forward<Injection_T>(injection)
    );
    self.m_invocations.push_back(create_injection_invocation<Injection_T>());

    return std::forward<Self_T>(self);
}

template <core::app::app_c App_T>
auto extensions::ResourceManager::operator()(App_T&& app) &&
{
    constexpr static auto transform_app{ []<typename X_App_T>(X_App_T&& x_app) {
        if constexpr (!core::app::has_addons_c<App_T, addons::ResourceManager>) {
            return std::forward<App_T>(x_app).template add_on<addons::ResourceManager>();
        }
        else {
            return std::forward<App_T>(x_app);
        }
    } };

    const auto invoke_plugins{
        [this]<typename AppWithResources_T>(AppWithResources_T&& app_with_resources) {
            std::ranges::for_each(
                std::move(m_invocations),
                std::bind_back(
                    &InjectionInvocation::operator(),
                    std::ref(m_injections),
                    std::ref(app_with_resources.resources)
                )
            );

            return std::forward<AppWithResources_T>(app_with_resources);
        }
    };

    return invoke_plugins(transform_app(std::forward<App_T>(app)));
}
