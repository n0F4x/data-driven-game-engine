#include "App.hpp"

#include <spdlog/spdlog.h>

auto App::create() -> Builder
{
    SPDLOG_TRACE("Creating App...");

    return Builder{};
}

auto App::Builder::build() -> App
{
    App app;

    // TODO: use std::bind_back
    for (PluginInvocation& invocation : m_invocations) {
        invocation(app);
    }

    return app;
}

auto App::Builder::PluginInvocation::operator()(App& app) -> void
{
    m_invocation(m_plugin_ref, app);
}
