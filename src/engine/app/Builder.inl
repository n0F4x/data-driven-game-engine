
#include "Builder.hpp"

namespace engine {

auto App::Builder::build_and_run(RunnerConcept<App> auto&& t_runner) && noexcept
    -> Result
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

template <typename Plugin>
auto App::Builder::add_plugin(auto&&... t_args) && noexcept -> App::Builder
{
    static_assert(PluginConcept<Plugin, decltype(t_args)...>);
    Plugin::setup(m_context, std::forward<decltype(t_args)>(t_args)...);
    return std::move(*this);
}

auto App::Builder::add_plugin(PluginConcept auto&& t_plugin) && noexcept
    -> App::Builder
{
    t_plugin.setup(m_context);
    return std::move(*this);
}

}   // namespace engine
