#include <functional>

import core.app;
import addons;
import extensions;

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

[[nodiscard]]
constexpr auto make_second(const First& first) -> Second
{
    return Second{ .ref = first.value };
}

auto main() -> int
{
    constexpr int result =
        core::app::Builder{}
            .extend_with<extensions::ResourceManager>()
            .use_resource(First{})
            .inject_resource([](const First& first) -> Second {
                return Second{ .ref = first.value };
            })
            .extend_with<extensions::Runnable>()
            .run([](auto app) {
                return app.resource_manager.template get<Second>().ref.get();
            });

    return result;
}
