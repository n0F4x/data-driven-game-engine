#include <iostream>

#include <app.hpp>
#include <plugins.hpp>

#include "core/cache/Cache.hpp"

#include "demo.hpp"

auto main() -> int
try {
    spdlog::set_level(spdlog::level::trace);

    const demo::ModelInfo model_info{
        //        "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb",
        //        "models/Avocado/glTF-Binary/Avocado.glb",
        //        "models/armor/armor.gltf",
        //        "models/DamagedHelmet.glb",
        //        "models/Sponza/glTF/Sponza.gltf",
        //        "models/SponzaKTX/Sponza01.gltf",
        "models/SponzaKTX/Sponza02.gltf",
        500,
        "shaders/pbr.frag.spv"
    };

    return App::create()
        .append([](App& app) {
            app.resources.emplace<core::cache::Cache>();
    })
        .append(plugins::Window{ .size = { 1'280, 720 }, .title = "My window" })
        .append_group(plugins::Renderer{}.require_vulkan_version(1, 1))
        .run(demo::run, model_info);
} catch (const std::exception& error) {
    try {
        // TODO: use std::println
        std::cout << error.what();
    } catch (...) {
        return -1;
    }
} catch (...) {
    return -2;
}
