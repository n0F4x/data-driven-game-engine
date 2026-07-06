#include <functional>
#include <memory_resource>
#include <string>

#include <catch2/catch_test_macros.hpp>

import ddge.modules.app.v2.registry.BuildableEntry;
import ddge.modules.app.v2.registry.BuildableEntryBuilder;
import ddge.modules.app.v2.registry.BuildDirector;
import ddge.modules.app.v2.registry.ConfigurationEntry;
import ddge.modules.app.v2.registry.CyclicDependencyDetected;
import ddge.modules.app.v2.registry.EntryBase;
import ddge.modules.app.v2.registry.EntryBuilderBase;
import ddge.modules.app.v2.registry.Registry;
import ddge.modules.app.v2.registry.RegistryBuilder;
import ddge.utility.containers.OptionalRef;
import ddge.utility.meta.reflection.name_of;

namespace ddge::app::v2 {

namespace {

const std::string type_name{ util::meta::name_of<RegistryBuilder>() };

template <typename Entry_T>
struct BuildDescriber {
    constexpr static auto operator()(BuildDirector<Entry_T>& build_director) -> void
    {
        build_director.template use_builder<typename Entry_T::Builder>();
    }
};

template <typename Builder_T>
struct BuilderBuildDescriber {
    constexpr static auto operator()(BuildDirector<Builder_T>& build_director) -> void
    {
        build_director.template use_function<Builder_T::create>();
    }
};

struct SelfBuildDependentEntry
    : BuildableEntry<SelfBuildDependentEntry, BuildDescriber<SelfBuildDependentEntry>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SelfBuildDependentEntry&) noexcept
            -> SelfBuildDependentEntry
        {
            return SelfBuildDependentEntry{};
        }
    };
};

struct SelfCreateDependentEntry
    : BuildableEntry<SelfCreateDependentEntry, BuildDescriber<SelfCreateDependentEntry>{}>   //
{
    struct Builder : BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}> {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto create(Builder&) noexcept -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> SelfCreateDependentEntry
        {
            return SelfCreateDependentEntry{};
        }
    };
};

struct OptionalSelfBuildDependentEntry
    : BuildableEntry<
          OptionalSelfBuildDependentEntry,
          BuildDescriber<OptionalSelfBuildDependentEntry>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(
            util::OptionalRef<OptionalSelfBuildDependentEntry>
        ) noexcept -> OptionalSelfBuildDependentEntry
        {
            return OptionalSelfBuildDependentEntry{};
        }
    };
};

struct OptionalSelfCreateDependentEntry
    : BuildableEntry<
          OptionalSelfCreateDependentEntry,
          BuildDescriber<OptionalSelfCreateDependentEntry>{}>   //
{
    struct Builder : BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}> {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto create(util::OptionalRef<Builder>) noexcept -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> OptionalSelfCreateDependentEntry
        {
            return OptionalSelfCreateDependentEntry{};
        }
    };
};

struct EntryCyclicBuildEntryA;
struct EntryCyclicBuildEntryB;

struct EntryCyclicBuildEntryA
    : BuildableEntry<EntryCyclicBuildEntryA, BuildDescriber<EntryCyclicBuildEntryA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(EntryCyclicBuildEntryB&) noexcept
            -> EntryCyclicBuildEntryA
        {
            return EntryCyclicBuildEntryA{};
        }
    };
};

struct EntryCyclicBuildEntryB
    : BuildableEntry<EntryCyclicBuildEntryB, BuildDescriber<EntryCyclicBuildEntryB>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(EntryCyclicBuildEntryA&) noexcept
            -> EntryCyclicBuildEntryB
        {
            return EntryCyclicBuildEntryB{};
        }
    };
};

struct BuilderCyclicBuildEntryA
    : BuildableEntry<BuilderCyclicBuildEntryA, BuildDescriber<BuilderCyclicBuildEntryA>{}>   //
{
    struct Builder;
};

struct BuilderCyclicBuildEntryB
    : BuildableEntry<BuilderCyclicBuildEntryB, BuildDescriber<BuilderCyclicBuildEntryB>{}>   //
{
    struct Builder;
};

struct EntryAndBuilderCyclicBuildEntryA
    : BuildableEntry<
          EntryAndBuilderCyclicBuildEntryA,
          BuildDescriber<EntryAndBuilderCyclicBuildEntryA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> EntryAndBuilderCyclicBuildEntryA
        {
            return EntryAndBuilderCyclicBuildEntryA{};
        }
    };
};

struct EntryAndBuilderCyclicBuildEntryB
    : BuildableEntry<
          EntryAndBuilderCyclicBuildEntryB,
          BuildDescriber<EntryAndBuilderCyclicBuildEntryB>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(
            EntryAndBuilderCyclicBuildEntryA&,
            const EntryAndBuilderCyclicBuildEntryA::Builder&
        ) noexcept -> EntryAndBuilderCyclicBuildEntryB
        {
            return EntryAndBuilderCyclicBuildEntryB{};
        }
    };
};

struct BuilderCyclicBuildEntryA::Builder : EntryBuilderBase {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build(const BuilderCyclicBuildEntryB::Builder&) noexcept
        -> BuilderCyclicBuildEntryA
    {
        return BuilderCyclicBuildEntryA{};
    }
};

struct BuilderCyclicBuildEntryB::Builder : EntryBuilderBase {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build(const BuilderCyclicBuildEntryA::Builder&) noexcept
        -> BuilderCyclicBuildEntryB
    {
        return BuilderCyclicBuildEntryB{};
    }
};

struct CyclicCreateEntryA
    : BuildableEntry<CyclicCreateEntryA, BuildDescriber<CyclicCreateEntryA>{}>   //
{
    struct Builder;
};

struct CyclicCreateEntryB
    : BuildableEntry<CyclicCreateEntryB, BuildDescriber<CyclicCreateEntryB>{}>   //
{
    struct Builder;
};

struct CyclicCreateEntryA::Builder
    : BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}> {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto create(CyclicCreateEntryB::Builder&) noexcept -> Builder
    {
        return Builder{};
    }

    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build() noexcept -> CyclicCreateEntryA
    {
        return CyclicCreateEntryA{};
    }
};

struct CyclicCreateEntryB::Builder
    : BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}> {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto create(CyclicCreateEntryA::Builder&) noexcept -> Builder
    {
        return Builder{};
    }

    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build() noexcept -> CyclicCreateEntryB
    {
        return CyclicCreateEntryB{};
    }
};

struct EntryDependencyA : EntryBase {};

struct EntryDependencyB
    : BuildableEntry<EntryDependencyB, BuildDescriber<EntryDependencyB>{}> {
    constexpr explicit EntryDependencyB(EntryDependencyA&) noexcept {}

    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(EntryDependencyA& a) noexcept -> EntryDependencyB
        {
            return EntryDependencyB{ a };
        }
    };
};

struct EntryBuilderDependencyA
    : BuildableEntry<EntryBuilderDependencyA, BuildDescriber<EntryBuilderDependencyA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> EntryBuilderDependencyA
        {
            return EntryBuilderDependencyA{};
        }
    };
};

struct EntryBuilderDependencyB
    : BuildableEntry<EntryBuilderDependencyB, BuildDescriber<EntryBuilderDependencyB>{}>   //
{
    constexpr explicit EntryBuilderDependencyB(
        const EntryBuilderDependencyA::Builder&
    ) noexcept
    {}

    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(
            const EntryBuilderDependencyA::Builder& a_builder
        ) noexcept -> EntryBuilderDependencyB
        {
            return EntryBuilderDependencyB{ a_builder };
        }
    };
};

struct BuilderEntryDependencyA : ConfigurationEntry {};

struct BuilderEntryDependencyB
    : BuildableEntry<BuilderEntryDependencyB, BuildDescriber<BuilderEntryDependencyB>{}>   //
{
    constexpr explicit BuilderEntryDependencyB(BuilderEntryDependencyA&) noexcept {}

    struct Builder : BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}> {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto create(BuilderEntryDependencyA& a) noexcept -> Builder
        {
            return Builder{ a };
        }

        std::reference_wrapper<BuilderEntryDependencyA> a;

        // ReSharper disable once CppDFAUnreachableFunctionCall
        constexpr explicit Builder(BuilderEntryDependencyA& a) : a{ a } {}

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr auto build() const noexcept -> BuilderEntryDependencyB
        {
            return BuilderEntryDependencyB{ a };
        }
    };
};

struct DependencyInversionEntryA
    : BuildableEntry<DependencyInversionEntryA, BuildDescriber<DependencyInversionEntryA>{}>   //
{
    struct Builder;
};

struct DependencyInversionEntryB
    : BuildableEntry<DependencyInversionEntryB, BuildDescriber<DependencyInversionEntryB>{}>   //
{
    struct Builder : BuildableEntryBuilder<Builder, BuilderBuildDescriber<Builder>{}> {
        [[nodiscard]]
        constexpr static auto create(DependencyInversionEntryA::Builder&) noexcept
            -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(DependencyInversionEntryA&) noexcept
            -> DependencyInversionEntryB
        {
            return DependencyInversionEntryB{};
        }
    };
};

struct DependencyInversionEntryA::Builder : EntryBuilderBase {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build(const DependencyInversionEntryB::Builder&) noexcept
        -> DependencyInversionEntryA
    {
        return DependencyInversionEntryA{};
    }
};

}   // namespace

TEST_CASE(type_name)
{
    std::pmr::unsynchronized_pool_resource transient_memory_resource{
        std::pmr::get_default_resource()
    };

#ifdef KILN_DEBUG
    SECTION("cyclic dependency detection")
    {
        SECTION("self")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<SelfBuildDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<SelfCreateDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }
        }

        SECTION("optional self")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<OptionalSelfBuildDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<OptionalSelfCreateDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }
        }

        SECTION("entry -> entry cycle")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<EntryCyclicBuildEntryA>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }

        SECTION("entry -> builder cycle")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<BuilderCyclicBuildEntryA>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }

        SECTION("entry -> (entry, builder)")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<EntryAndBuilderCyclicBuildEntryB>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }

        SECTION("builder -> builder cycle")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<CyclicCreateEntryA>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }
    }
#endif

    SECTION("entry -> entry dependency")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<EntryDependencyA>();
            registry_builder.register_entry<EntryDependencyB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<EntryDependencyA>());
            REQUIRE(registry.contains<EntryDependencyB>());
        }

        SECTION("reordered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<EntryDependencyB>();
            registry_builder.register_entry<EntryDependencyA>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<EntryDependencyA>());
            REQUIRE(registry.contains<EntryDependencyB>());
        }

        SECTION("automatically registered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<EntryDependencyB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<EntryDependencyA>());
            REQUIRE(registry.contains<EntryDependencyB>());
        }
    }

    SECTION("entry -> builder dependency")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<EntryBuilderDependencyA>();
            registry_builder.register_entry<EntryBuilderDependencyB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<EntryBuilderDependencyA>());
            REQUIRE(registry.contains<EntryBuilderDependencyB>());
        }

        SECTION("reordered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<EntryBuilderDependencyB>();
            registry_builder.register_entry<EntryBuilderDependencyA>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<EntryBuilderDependencyA>());
            REQUIRE(registry.contains<EntryBuilderDependencyB>());
        }

        SECTION("automatically registered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<EntryBuilderDependencyB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<EntryBuilderDependencyA>());
            REQUIRE(registry.contains<EntryBuilderDependencyB>());
        }
    }

    SECTION("builder -> (configuration) entry dependency")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<BuilderEntryDependencyA>();
            registry_builder.register_entry<BuilderEntryDependencyB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<BuilderEntryDependencyA>());
            REQUIRE(registry.contains<BuilderEntryDependencyB>());
        }

        SECTION("reordered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<BuilderEntryDependencyB>();
            registry_builder.register_entry<BuilderEntryDependencyA>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<BuilderEntryDependencyA>());
            REQUIRE(registry.contains<BuilderEntryDependencyB>());
        }

        SECTION("automatically registered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<BuilderEntryDependencyB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<BuilderEntryDependencyA>());
            REQUIRE(registry.contains<BuilderEntryDependencyB>());
        }
    }

    SECTION("entryB -> entryA -> builderB -> builderA)")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<DependencyInversionEntryA>();
            registry_builder.register_entry<DependencyInversionEntryB>();
            Registry registry =
                std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<DependencyInversionEntryA>());
            REQUIRE(registry.contains<DependencyInversionEntryB>());
        }
    }
}

}   // namespace ddge::app::v2
