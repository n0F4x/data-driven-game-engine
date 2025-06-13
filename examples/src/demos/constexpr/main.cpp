#include <functional>

import core.app;

import extensions.Resources;
import extensions.Runnable;

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

auto main() -> int
{
    constexpr int result =
        core::app::create()
            .extend_with(extensions::Resources{})
            .use_resource(First{})
            .inject_resource([](const First& first) -> Second {
                return Second{ .ref = first.value };
            })
            .extend_with(extensions::Runnable{})
            .run([](auto app) {
                return app.resource_manager.template get<Second>().ref.get();
            });

    return result;
}
