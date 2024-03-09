#pragma once

#include <concepts>

#include "Store.hpp"

namespace engine {

template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, Store&, Args...>;

}   // namespace engine
