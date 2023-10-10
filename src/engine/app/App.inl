#include <functional>

#include <spdlog/spdlog.h>

namespace engine {

template <typename... Args>
auto App::run(RunnerConcept<Args...> auto&& t_runner, Args&&... t_args) noexcept
    -> void
{
    SPDLOG_INFO("App is running");
    std::invoke(t_runner, *this, std::forward<Args>(t_args)...);
}

}   // namespace engine
