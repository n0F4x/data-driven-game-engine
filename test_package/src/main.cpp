import core.app;

import extensions.Resources;

auto main() -> int
{
    return core::app::create()
        .extend_with(extensions::Resources{})
        .use_resource(int{})
        .build()
        .resource_manager.get<int>();
}
