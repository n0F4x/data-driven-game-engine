module;

#include <spdlog/spdlog.h>

module core.app.Builder;

namespace core::app {

auto Builder::build() -> App
{
    App app;

    std::ranges::for_each(
        m_invocations, std::bind_back(&PluginInvocation::operator(), std::ref(app))
    );

    return app;
}

auto PluginInvocation::operator()(App& app) -> void
{
    m_invocation(m_plugin_ref, app);
}

auto create() -> Builder
{
    SPDLOG_TRACE("Creating App...");

    return Builder{};
}

}   // namespace core::app
