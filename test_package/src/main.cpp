import app;

import plugins.resources;

auto main() -> int
{
    return app::create()
        .plug_in(plugins::resources)
        .insert_resource(int{})
        .build()
        .resource_manager.get<int>();
}
