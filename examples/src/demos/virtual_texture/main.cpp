#include <print>

#include <spdlog/spdlog.h>

#include <base/DemoBase.hpp>
#include <plugins.hpp>

#include "demo.hpp"
#include "DemoApp.hpp"


auto main() -> int
try {
    spdlog::set_level(spdlog::level::trace);

    App::create()
        .use(plugins::Window{
            .size  = { 1'280, 720 },
            .title = "Virtual texturing demo",
    })
        .apply(plugins::Renderer{})
        .use(examples::base::DemoBasePlugin{ .movement_speed = 1.f })
        .use(demo::DemoPlugin{})
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
