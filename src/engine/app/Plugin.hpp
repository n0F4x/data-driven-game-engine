#pragma once

#include <concepts>

#include "App.hpp"
#include "Store.hpp"

namespace engine {

template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App::Builder&, Args...>;

}   // namespace engine
