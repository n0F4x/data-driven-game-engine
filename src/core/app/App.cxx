module;

#include <spdlog/spdlog.h>

module core.app.App;

auto App::create() -> Builder
{
    SPDLOG_TRACE("Creating App...");

    return Builder{};
}

auto App::Builder::build() -> App
{
    App app;

    std::ranges::for_each(
        m_invocations, std::bind_back(&PluginInvocation::operator(), std::ref(app))
    );

    return app;
}

auto App::Builder::PluginInvocation::operator()(App& app) -> void
{
    m_invocation(m_plugin_ref, app);
}
