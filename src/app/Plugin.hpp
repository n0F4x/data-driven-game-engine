#pragma once

#include <concepts>

#include "App.hpp"
#include "store/Store.hpp"

namespace app {

template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App::Builder&, Args...>;

}   // namespace app
