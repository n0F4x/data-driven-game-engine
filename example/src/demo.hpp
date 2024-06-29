#pragma once

#include <filesystem>

#include <app.hpp>

namespace demo {

struct ModelInfo {
    std::filesystem::path filepath;
    float                 movement_speed{ 5.f };
    std::filesystem::path fragment_shader{ "shaders/pbr.frag.spv" };
};

auto run(app::App& t_app, const ModelInfo& t_model_info) -> int;

}   // namespace demo
