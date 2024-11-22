module;

#include <any>

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

export module app.App;

import core.utility.meta.functional;
import core.utility.meta.tuple_like;
import core.utility.tuple;

import store.Store;
import store.StoreView;

export class App {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///-------------///
    ///  Variables  ///
    ///-------------///
    Store          resources;
    entt::registry registry;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]]
    static auto create() -> Builder;
};

template <typename Plugin>
concept PluginConcept = !std::is_void_v<
    core::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Plugin>>>>;

template <typename Modifier>
concept ModifierConcept = requires(Modifier&& modifier, App::Builder& builder) {
    std::invoke(std::forward<Modifier>(modifier), builder);
};

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner&&, App&&, Args&&...>;

class App::Builder {
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
    class PluginInvocation;

    Store                         m_plugins;
    std::vector<PluginInvocation> m_invocations;
};

class App::Builder::PluginInvocation {
public:
    template <typename Plugin>
    explicit PluginInvocation(Plugin& plugin_ref);

    auto operator()(App& app) -> void;

private:
    std::any                             m_plugin_ref;
    std::function<void(std::any&, App&)> m_invocation;
};

#include "App.inl"
