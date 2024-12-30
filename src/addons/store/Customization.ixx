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

export template <typename Plugin_T>
concept plugin_c = !std::is_void_v<
    utils::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Plugin_T>>>>;

class PluginInvocation {
public:
    template <typename Plugin_T>
    explicit PluginInvocation(Plugin_T& plugin_ref);

    auto operator()(core::store::Store& store) -> void;

private:
    std::any                                            m_plugin_ref;
    std::function<void(std::any&, core::store::Store&)> m_invocation;
};

export class Customization {
public:
    template <core::app::builder_c Self_T, plugin_c Plugin_T>
    auto inject(this Self_T&&, Plugin_T&& plugin) -> Self_T;

protected:
    template <core::app::app_c App_T>
    auto operator()(App_T&& app) &&;

private:
    core::store::Store            m_store;
    std::vector<PluginInvocation> m_invocations;
};

}   // namespace addons::store

template <typename Callable_T>
auto gather_resources(core::store::Store& store)
{
    using RequiredResourcesTuple = utils::meta::arguments_of_t<Callable_T>;
    return utils::generate_tuple<RequiredResourcesTuple>(
        [&store]<typename Resource>() -> Resource {
            return store.at<std::remove_cvref_t<Resource>>();
        }
    );
}

template <typename Plugin_T>
addons::store::PluginInvocation::PluginInvocation(Plugin_T& plugin_ref)
    : m_plugin_ref{ std::ref(plugin_ref) },
      m_invocation{ [](std::any& erased_plugin_ref, core::store::Store& store) {
          using ResourceType = utils::meta::
              invoke_result_of_t<std::remove_pointer_t<std::decay_t<Plugin_T>>>;
          store.emplace<ResourceType>(std::apply(
              std::any_cast<std::reference_wrapper<Plugin_T>>(erased_plugin_ref),
              gather_resources<std::remove_pointer_t<std::decay_t<Plugin_T>>>(store)
          ));
      } }
{}

template <core::app::builder_c Self_T, addons::store::plugin_c Plugin_T>
auto addons::store::Customization::inject(this Self_T&& self, Plugin_T&& plugin) -> Self_T
{
    if constexpr (requires { requires std::is_function_v<decltype(Plugin_T::setup)>; }) {
        std::apply(
            Plugin_T::setup, gather_resources<decltype(Plugin_T::setup)>(self.m_store)
        );
    }
    else if constexpr (requires {
                           requires std::
                               is_member_function_pointer_v<decltype(&Plugin_T::setup)>;
                       })
    {
        std::apply(
            std::bind_front(&Plugin_T::setup, plugin),
            gather_resources<decltype(&Plugin_T::setup)>(self.m_store)
        );
    }

    self.m_invocations.emplace_back(self.m_store.template emplace<std::decay_t<Plugin_T>>(
        std::forward<Plugin_T>(plugin)
    ));

    return std::forward<Self_T>(self);
}

template <core::app::app_c App_T>
auto addons::store::Customization::operator()(App_T&& app) &&
{
    std::ranges::for_each(
        std::move(m_invocations),
        std::bind_back(&PluginInvocation::operator(), std::ref(m_store))
    );

    return std::forward<App_T>(app).template mix<Mixin>(std::move(m_store));
}
