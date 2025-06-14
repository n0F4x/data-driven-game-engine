#include <functional>

import app;

import plugins.resources;
import plugins.runnable;

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

auto main() -> int
{
    constexpr int result =
        app::create()
            .plug_in(plugins::resources)
            .insert_resource(First{})
            .inject_resource([](const First& first) -> Second {
                return Second{ .ref = first.value };
            })
            .plug_in(plugins::runnable)
            .run([](auto app) {
                return app.resource_manager.template get<Second>().ref.get();
            });

    return result;
}
