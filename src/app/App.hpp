#pragma once

#include <any>
#include <tuple>

#include <entt/entity/registry.hpp>

#include "core/store/Store.hpp"

class App {
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
concept PluginConcept = requires(Plugin&& plugin, App& app) {
    std::bind_front(
        &std::remove_cvref_t<Plugin>::operator(),
        std::forward<Plugin>(plugin),
        std::ref(app)
    );
};

template <typename PluginGroup>
concept ModifierConcept = requires(PluginGroup&& plugin_group, App::Builder& builder) {
    std::invoke(std::forward<PluginGroup>(plugin_group), builder);
};

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner&&, App&&, Args&&...>;

class App::Builder {
public:
    template <PluginConcept Plugin, typename Self, typename... Args>
    auto use(this Self&&, Args&&... args) -> Self;
    template <PluginConcept Plugin, typename Self>
    auto use(this Self&&, Plugin&& plugin) -> Self;

    template <ModifierConcept Modifier, typename Self, typename... Args>
    auto apply(this Self&&, Args&&... args) -> Self;
    template <ModifierConcept Modifier, typename Self>
    auto apply(this Self&&, Modifier&& plugin_group) -> Self;

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
