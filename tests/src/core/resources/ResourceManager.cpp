#include <functional>

import ddge.modules.resources.ResourceManager;

namespace {

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

[[nodiscard]]
constexpr auto make_first() -> First
{
    return First{};
}

[[nodiscard]]
constexpr auto make_second(const First& first) -> Second
{
    return Second{ .ref = first.value };
}

}   // namespace

static_assert(
    [] {
        ddge::resource::ResourceManager<First, Second> resource_manager{ make_first,
                                                                         make_second };
        auto moved_resource_manager{ std::move(resource_manager) };

        return moved_resource_manager.get<First>().value
            == moved_resource_manager.get<Second>().ref.get();
    }(),
    "move test failed"
);
