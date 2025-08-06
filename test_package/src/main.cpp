import app;

import plugins.resources;

auto main() -> int
{
    return app::create()
        .plug_in(plugins::Resources{})
        .insert_resource(int{})
        .build()
        .resource_manager.at<int>();
}
