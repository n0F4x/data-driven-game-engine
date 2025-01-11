module;

#include <algorithm>
#include <any>
#include <functional>
#include <type_traits>

export module addons.store.Customization;

import core.app.App;
import core.app.Builder;
import core.store.Store;

import utility.meta.functional.arguments_of;
import utility.meta.functional.invoke_result_of;

import utility.tuple;

import addons.store.Mixin;

namespace addons::store {

export template <typename Resource_T>
concept resource_c = core::store::storable_c<Resource_T>;

export template <typename Injection_T>
concept injection_c = resource_c<
    utils::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Injection_T>>>>;

class InjectionInvocation {
public:
    template <typename Injection_T>
    explicit InjectionInvocation(Injection_T& injection_ref);

    auto operator()(core::store::Store& store) -> void;

private:
    std::any                                            m_injection_ref;
    std::function<void(std::any&, core::store::Store&)> m_invocation;
};

export class Customization {
public:
    template <typename Self_T, typename Resource_T>
    auto use(this Self_T&&, Resource_T&& resource) -> Self_T;

    template <typename Self_T, injection_c Injection_T>
    auto inject(this Self_T&&, Injection_T&& injection) -> Self_T;

protected:
    template <core::app::app_c App_T>
    auto operator()(App_T&& app) &&;

private:
    core::store::Store               m_injections;
    std::vector<InjectionInvocation> m_invocations;
};

}   // namespace addons::store

template <typename Callable_T>
auto gather_parameters(core::store::Store& store)
{
    using RequiredResourcesTuple = utils::meta::arguments_of_t<Callable_T>;
    return utils::generate_tuple<RequiredResourcesTuple>(
        [&store]<typename Resource>() -> Resource {
            return store.at<std::remove_cvref_t<Resource>>();
        }
    );
}

template <typename Injection_T>
addons::store::InjectionInvocation::InjectionInvocation(Injection_T& injection_ref)
    : m_injection_ref{ std::ref(injection_ref) },
      m_invocation{ [](std::any& erased_injection_ref, core::store::Store& store) {
          using ResourceType = utils::meta::
              invoke_result_of_t<std::remove_pointer_t<std::decay_t<Injection_T>>>;
          store.emplace<ResourceType>(std::apply(
              std::any_cast<std::reference_wrapper<Injection_T>>(erased_injection_ref),
              ::gather_parameters<std::remove_pointer_t<std::decay_t<Injection_T>>>(store)
          ));
      } }
{}

template <typename Self_T, typename Resource_T>
auto addons::store::Customization::use(this Self_T&& self, Resource_T&& resource)
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

    return std::forward<Self_T>(self).inject(
        Injection{ std::forward<Resource_T>(resource) }
    );
}

template <typename Self_T, addons::store::injection_c Injection_T>
auto addons::store::Customization::inject(this Self_T&& self, Injection_T&& injection)
    -> Self_T
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

    self.m_invocations.emplace_back(
        self.m_injections.template emplace<std::decay_t<Injection_T>>(
            std::forward<Injection_T>(injection)
        )
    );

    return std::forward<Self_T>(self);
}

template <core::app::app_c App_T>
auto addons::store::Customization::operator()(App_T&& app) &&
{
    core::store::Store store;

    std::ranges::for_each(
        std::move(m_invocations),
        std::bind_back(&InjectionInvocation::operator(), std::ref(store))
    );

    return std::forward<App_T>(app).template mix<Mixin>(std::move(store));
}
