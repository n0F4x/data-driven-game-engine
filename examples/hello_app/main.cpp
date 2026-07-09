#include <cstdio>

import ddge.app.v2;
import ddge.registry;
import ddge.util.containers.OptionalRef;

struct GraphicsSystemIntegration : ddge::registry::EntryBase {};

template <typename Entry_T>
struct BuildDescriber {
    constexpr static auto operator()(
        ddge::registry::BuildDirector<Entry_T>& build_director
    ) -> void
    {
        build_director.template use_builder<typename Entry_T::Builder>();
    }
};

template <typename EntryBuilder_T>
struct BuilderBuildDescriber {
    constexpr static auto operator()(
        ddge::registry::BuildDirector<EntryBuilder_T>& build_director
    ) -> void
    {
        build_director.template use_function<EntryBuilder_T::create>();
    }
};

struct WindowSystem
    : ddge::registry::BuildableEntry<WindowSystem, BuildDescriber<WindowSystem>{}> {
    struct Builder;

    GraphicsSystemIntegration* graphics_system{};
};

struct WindowSystem::Builder : ddge::registry::EntryBuilderBase {
    bool graphics_support_requested = false;

    auto build(
        const ddge::util::OptionalRef<GraphicsSystemIntegration> graphics_system_integration
    ) const -> WindowSystem
    {
        if (graphics_support_requested && graphics_system_integration.has_value()) {
            return WindowSystem{ .graphics_system = &*graphics_system_integration };
        }
        return WindowSystem{};
    }
};

struct RenderSystem
    : ddge::registry::BuildableEntry<RenderSystem, BuildDescriber<RenderSystem>{}>   //
{
    struct Builder;

    GraphicsSystemIntegration& graphics_system;
    WindowSystem*              window_system{};
};

struct RenderSystem::Builder
    : ddge::registry::BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}>   //
{
    static auto create(const ddge::util::OptionalRef<WindowSystem::Builder> window_builder)
        -> Builder
    {
        if (window_builder.has_value()) {
            window_builder->graphics_support_requested = true;
        }

        return Builder{};
    }

    static auto build(
        GraphicsSystemIntegration&                  graphics_system_integration,
        const ddge::util::OptionalRef<WindowSystem> window_system
    ) -> RenderSystem
    {
        return RenderSystem{
            .graphics_system = graphics_system_integration,
            .window_system   = window_system.has_value() ? &*window_system : nullptr,
        };
    }
};

auto main() -> int
{
    /*
     * GraphicsSystemIntegration gets implicitly injected
     *  as RenderSystem unconditionally depends on it.
     * WindowSystem gets built before RenderSystem
     *  as RenderSystem (conditionally) depends on it.
     */
    ddge::app::v2::App app = ddge::app::v2::create()
                                 .register_entry<RenderSystem>()
                                 .register_entry<WindowSystem>()
                                 .build();

    /*
     * RenderSystem is never headless when the WindowSystem is present
     */
    std::puts(
        app.registry().at<RenderSystem>().window_system == nullptr
            ? "Renderer is headless"
            : "Renderer is not headless"
    );
}
