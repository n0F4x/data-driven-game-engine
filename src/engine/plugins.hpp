#pragma once

#include "engine/asset_manager/Plugin.hpp"
#include "engine/logger/Plugin.hpp"
#include "engine/renderer/Plugin.hpp"
#include "engine/scene_graph/Plugin.hpp"
#include "engine/window/Plugin.hpp"

namespace engine::plugins {

using AssetManager = asset_manager::Plugin;
using Logger       = logger::Plugin;
using Renderer     = renderer::Plugin;
using SceneGraph   = scene_graph::Plugin;
using Window       = window::Plugin;

}   // namespace engine::plugins
