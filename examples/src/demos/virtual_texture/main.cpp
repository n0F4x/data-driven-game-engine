#include <print>

#include <spdlog/spdlog.h>

import core;
import addons;

import utility.Size;

import examples.base.DemoBase;

import demos.virtual_texture;
import demos.virtual_texture.DemoApp;


auto main() -> int
try {
    spdlog::set_level(spdlog::level::trace);

    core::app::create()
        .customize<addons::functional::Customization>()
        .customize<addons::store::Customization>()
        .customize<addons::runnable::Customization>()
        .use(core::window::Window(utils::Size2i{ 1'280, 720 }, "Virtual texturing demo"))
        .apply(addons::Renderer{})
        .inject(examples::base::DemoBasePlugin{ .movement_speed = 1.f })
        .inject(demo::DemoPlugin{})
        .run(demo::run);

} catch (const std::exception& error) {
    try {
        std::println("{}", error.what());
        return -1;
    } catch (...) {
        return -3;
    }
} catch (...) {
    return -2;
}
