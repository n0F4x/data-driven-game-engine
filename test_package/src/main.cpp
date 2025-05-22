import core.app;

import extensions.ResourceManager;

auto main() -> int
{
    return core::app::create()
        .extend_with(extensions::ResourceManager{})
        .use_resource(int{})
        .build()
        .resource_manager.get<int>();
}
