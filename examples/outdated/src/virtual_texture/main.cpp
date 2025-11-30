#include <print>

import ddge.prelude;
import ddge.modules;
import ddge.utility.Size;

import examples.base.DemoBase;

import demos.virtual_texture;
import demos.virtual_texture.DemoApp;


auto main() -> int
try {
    ddge::app::create()
        .plug_in(ddge::app::FunctionalPlugin{})
        .plug_in(ddge::resources::Plugin{})
        .plug_in(ddge::app::RunnablePlugin{})
        .insert_resource(
            ddge::window::Window(ddge::util::Size2i{ 1'280, 720 }, "Virtual texturing demo")
        )
        .transform(ddge::renderer::setup)
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
