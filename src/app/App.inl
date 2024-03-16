#include <functional>

#include "spdlog/spdlog.h"

namespace app {

template <typename... Args>
auto App::run(RunnerConcept<Args...> auto&& t_runner, Args&&... t_args)
    -> std::invoke_result_t<decltype(t_runner), App&, Args...>
{
    SPDLOG_INFO("App is running");
    return std::invoke(
        std::forward<decltype(t_runner)>(t_runner), *this, std::forward<Args>(t_args)...
    );
}

}   // namespace core
