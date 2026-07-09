#include <functional>

import ddge.app.v2;

struct First : ddge::app::v2::EntryBase {
    int value{ 42 };
};

struct Second;

auto describe_second_build(ddge::app::v2::BuildDirector<Second>&) -> void;

struct Second : ddge::app::v2::BuildableEntry<Second, describe_second_build> {
    std::reference_wrapper<const int> ref;
};

auto build_second(const First& first) -> Second
{
    return Second{ .ref = first.value };
}

auto describe_second_build(ddge::app::v2::BuildDirector<Second>& build_director) -> void
{
    build_director.use_function<build_second>();
}

auto main() -> int
{
    const ddge::app::v2::App app = ddge::app::v2::create()   //
                                       .register_entry<Second>()
                                       .build();

    return app.registry().at<Second>().ref.get();
}
