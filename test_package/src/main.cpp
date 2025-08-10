import ddge.prelude;

auto main() -> int
{
    return ddge::app::create()
        .plug_in(ddge::plugins::Resources{})
        .insert_resource(int{})
        .build()
        .resource_manager.at<int>();
}
