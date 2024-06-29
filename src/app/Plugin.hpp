#pragma once

#include "App.hpp"

template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App::Builder&, Args...>;
