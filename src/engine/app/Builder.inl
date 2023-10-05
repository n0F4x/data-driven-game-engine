
#include "Builder.hpp"

namespace engine {

auto App::Builder::build_and_run(const RunnerConcept<App> auto& t_runner
) && noexcept -> Result
{
    if (auto app{ std::move(*this).build() }) {
        app->run(t_runner);
        return Result::eSuccess;
    }
    return Result::eFailure;
}

template <PluginConcept Plugin>
auto App::Builder::add_plugin() && noexcept -> App::Builder
{
    Plugin::setup(m_context);
    return std::move(*this);
}

auto App::Builder::add_plugin(const PluginConcept auto& t_plugin) && noexcept
    -> App::Builder
{
    t_plugin.setup(m_context);
    return std::move(*this);
}

}   // namespace engine
