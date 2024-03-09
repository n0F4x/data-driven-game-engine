#pragma once

#include <concepts>

#include "store/Store.hpp"

#include "App.hpp"

namespace app {

template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App::Builder&, Args...>;

}   // namespace engine
