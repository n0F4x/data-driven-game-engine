#include <print>

import app;
import core;
import addons;
import extensions;
import plugins;

import utility.Size;

import examples.base.DemoBase;

import demos.virtual_texture;
import demos.virtual_texture.DemoApp;


auto main() -> int
try {
    app::create()
        .plug_in(plugins::functional)
        .plug_in(plugins::Resources{})
        .plug_in(plugins::runnable)
        .insert_resource(
            core::window::Window(util::Size2i{ 1'280, 720 }, "Virtual texturing demo")
        )
        .transform(extensions::renderer::setup)
        .inject_resource(examples::base::DemoBasePlugin{ .movement_speed = 1.f })
        .inject_resource(demo::DemoPlugin{})
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
