module;

#include <utility>

export module ddge.modules.app.v2.App;

import ddge.modules.app.v2.memory.MemoryArena;
import ddge.modules.app.v2.registry.Registry;
import ddge.util.meta.type_traits.forward_like;

namespace ddge::app::v2 {

export class App {
public:
    explicit App(MemoryArena&& memory_arena, Registry&& registry);

    template <typename Self_T>
    [[nodiscard]]
    auto registry(this Self_T&&) noexcept -> util::meta::forward_like_t<Registry, Self_T>;

private:
    MemoryArena m_arena;
    Registry    m_registry;
};

}   // namespace ddge::app::v2

namespace ddge::app::v2 {

inline App::App(MemoryArena&& memory_arena, Registry&& registry)
    : m_arena{ memory_arena },
      m_registry{ std::move(registry) }
{}

template <typename Self_T>
auto App::registry(this Self_T&& self) noexcept
    -> util::meta::forward_like_t<Registry, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_registry);
}

}   // namespace ddge::app::v2
