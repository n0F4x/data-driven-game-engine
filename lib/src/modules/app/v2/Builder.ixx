module;

#include <concepts>
#include <memory_resource>
#include <utility>

export module ddge.modules.app.v2.Builder;

import ddge.modules.app.v2.App;
import ddge.modules.app.v2.config.Config;
import ddge.modules.app.v2.memory.MemoryArena;
import ddge.modules.app.v2.registry.entry_c;
import ddge.modules.app.v2.registry.Registry;
import ddge.modules.app.v2.registry.RegistryBuilder;

namespace ddge::app::v2 {

export class Builder {
public:
    explicit Builder(const Config& config = Config{});


    template <entry_c Entry_T, typename Self_T>
    auto register_entry(this Self_T&&) -> Self_T&&;

    template <typename Self_T, typename Bundle_T>
        requires std::invocable<Bundle_T&&, Builder&>
    auto apply_bundle(this Self_T&&, Bundle_T&& bundle) -> Self_T&&;


    [[nodiscard]]
    auto build() && -> App;

private:
    MemoryArena     m_arena;
    RegistryBuilder m_registry_builder;
};

}   // namespace ddge::app::v2

namespace ddge::app::v2 {

Builder::Builder(const Config& config)
    : m_registry_builder{
          std::allocator_arg,
          m_arena.pool_allocator(),
          auto{ m_arena },
          config,
      }
{
}

template <entry_c Entry_T, typename Self_T>
auto Builder::register_entry(this Self_T&& self) -> Self_T&&
{
    self.Builder::m_registry_builder.template register_entry<Entry_T>();
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename Bundle_T>
    requires std::invocable<Bundle_T&&, Builder&>
auto Builder::apply_bundle(this Self_T&& self, Bundle_T&& bundle) -> Self_T&&
{
    std::invoke(std::forward<Bundle_T>(bundle), self);
    return std::forward<Self_T>(self);
}

auto Builder::build() && -> App
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        m_arena.make_transient_resource()
    };

    Registry registry{ std::move(m_registry_builder).build(transient_memory_resource) };

    return App{ std::move(m_arena), std::move(registry) };
}

}   // namespace ddge::app::v2
