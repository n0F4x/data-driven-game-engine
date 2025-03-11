#include "log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

auto logger{ spdlog::stdout_color_mt("engine_internal") };

auto core::log::internal::logger() -> spdlog::logger&
{
    return *::logger;
}
