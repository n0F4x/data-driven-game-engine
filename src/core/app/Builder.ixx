module;

#include <any>
#include <functional>
#include <type_traits>
#include <vector>

#include <spdlog/spdlog.h>

export module core.app.Builder;

import core.app.App;
import core.store.Store;
import core.store.StoreView;

import utility.meta.functional.arguments_of;
import utility.meta.functional.invoke_result_of;
import utility.meta.tuple_like;
import utility.tuple;

namespace core::app {

export class Builder;

template <typename Plugin>
concept PluginConcept = !std::is_void_v<
    utils::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Plugin>>>>;

template <typename Modifier>
concept ModifierConcept = requires(Modifier&& modifier, Builder& builder) {
    std::invoke(std::forward<Modifier>(modifier), builder);
};

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner&&, App&&, Args&&...>;

class PluginInvocation {
public:
    template <typename Plugin>
    explicit PluginInvocation(Plugin& plugin_ref);

    auto operator()(App& app) -> void;

private:
    std::any                             m_plugin_ref;
    std::function<void(std::any&, App&)> m_invocation;
};

export class Builder {
public:
    template <typename Self, PluginConcept Plugin>
    auto use(this Self&&, Plugin&& plugin) -> Self;

    template <typename Self, ModifierConcept Modifier>
    auto apply(this Self&&, Modifier&& modifier) -> Self;

    [[nodiscard]]
    auto build() -> App;

    template <typename Runner, typename... Args>
        requires(RunnerConcept<Runner, Args...>)
    auto run(Runner&& runner, Args&&... args)
        -> std::invoke_result_t<Runner&&, App&&, Args&&...>;

private:
    Store                         m_plugins;
    std::vector<PluginInvocation> m_invocations;
};

export [[nodiscard]]
auto create() -> Builder;

}   // namespace core::app

#include "Builder.inl"
