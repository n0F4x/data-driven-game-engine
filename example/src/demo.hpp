#pragma once

#include <filesystem>

#include <app.hpp>

namespace demo {

struct ModelInfo {
    explicit ModelInfo(const std::filesystem::path& t_filepath, float t_movement_speed = 5.f)
        : filepath{ t_filepath },
          movement_speed{ t_movement_speed }
    {}

    std::filesystem::path filepath;
    float                 movement_speed;
};

auto run(app::App& t_app, const ModelInfo& t_model_info) noexcept -> int;

}   // namespace demo
