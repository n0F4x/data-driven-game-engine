module;

#include <utility>

export module ddge.app.v2.App;

import ddge.app.v2.memory.MemoryArena;
import ddge.registry.Registry;
import ddge.util.meta.type_traits.forward_like;

namespace ddge::app::v2 {

export class App {
public:
    explicit App(MemoryArena&& memory_arena, registry::Registry&& registry);

    template <typename Self_T>
    [[nodiscard]]
    auto registry(this Self_T&&) noexcept
        -> util::meta::forward_like_t<registry::Registry, Self_T>;

private:
    MemoryArena        m_arena;
    registry::Registry m_registry;
};

}   // namespace ddge::app::v2

namespace ddge::app::v2 {

inline App::App(MemoryArena&& memory_arena, registry::Registry&& registry)
    : m_arena{ memory_arena },
      m_registry{ std::move(registry) }
{}

template <typename Self_T>
auto App::registry(this Self_T&& self) noexcept
    -> util::meta::forward_like_t<registry::Registry, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_registry);
}

}   // namespace ddge::app::v2
