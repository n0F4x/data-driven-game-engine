#pragma once

#include <concepts>

#include "App.hpp"

namespace engine {

template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App::Context&, Args...>;

}   // namespace engine
