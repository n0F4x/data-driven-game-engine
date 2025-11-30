#include <functional>

import ddge.prelude;

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

auto main() -> int
{
    // ordered_map is not constexpr 😢
    const int result = ddge::app::create()
                     .plug_in(ddge::resources::Plugin{})
                     .insert_resource(First{})
                     .inject_resource([](const First& first) -> Second {
                         return Second{ .ref = first.value };
                     })
                     .plug_in(ddge::app::RunnablePlugin{})
                     .run([](auto app) {
                         return app.resource_manager.template at<Second>().ref.get();
                     });

    return result;
}
