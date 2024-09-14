#pragma once

#include <filesystem>

#include <app.hpp>

namespace demo {

struct ModelInfo {
    std::filesystem::path filepath;
    float                 movement_speed{ 5.f };
};

auto run(App app, const ModelInfo& model_info) -> int;

}   // namespace demo
