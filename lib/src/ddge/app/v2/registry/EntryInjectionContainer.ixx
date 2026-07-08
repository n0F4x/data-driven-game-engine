module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <string>
#include <vector>

export module ddge.app.v2.registry.EntryInjectionContainer;

import ddge.app.v2.registry.ConfigurationEntry;
import ddge.app.v2.registry.DependencyChainNode;
import ddge.app.v2.registry.EntryBuilderBase;
import ddge.app.v2.registry.EntryBuilderContainer;
import ddge.app.v2.registry.strip_dependency_t;
import ddge.app.v2.registry.Registry;
import ddge.util.meta.concepts.functional.function;
import ddge.util.meta.concepts.functional.function_pointer;
import ddge.util.meta.concepts.specialization_of;
import ddge.util.containers.MoveOnlyFunction;
import ddge.util.containers.OptionalRef;
import ddge.util.meta.reflection.hash;
import ddge.util.meta.reflection.name_of;
import ddge.util.meta.type_traits.forward_like;
import ddge.util.meta.type_traits.functional.arguments_of;
import ddge.util.meta.type_traits.functional.result_of;
import ddge.util.TypeList;

namespace ddge::app::v2 {

using ErasedEntryInjection =
    util::MoveOnlyFunction<auto(EntryBuilderContainer&, Registry&) &&->void, 0>;

export class EntryInjectionContainer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    EntryInjectionContainer(EntryInjectionContainer&&, const allocator_type&);

    explicit EntryInjectionContainer() = default;
    explicit EntryInjectionContainer(const allocator_type&);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <util::meta::function_c Injection_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;


    template <auto injection_T>
    auto try_insert() -> bool;

    auto build(
        EntryBuilderContainer&     builders,
        Registry&                  registry,
        std::pmr::memory_resource& transient_memory_resource
    ) && -> void;

private:
    std::pmr::vector<ErasedEntryInjection>       m_injections;
    std::pmr::vector<uint64_t>                   m_builder_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_dependency_hashes;
    std::pmr::vector<std::optional<std::pmr::vector<uint64_t>>> m_dependent_builder_hashes;
#ifdef ENGINE_DEBUG
    std::pmr::vector<std::pmr::string> m_builder_names;
#endif


    auto sort() -> void;

    [[nodiscard]]
    auto check_cyclic_dependencies(
        std::pmr::memory_resource& transient_memory_resource
    ) const -> bool;

    auto collect_dependent_builder_hashes() -> void;
    auto bubble_up_dependencies_of(std::size_t index) -> void;


    [[nodiscard]]
    auto check_cyclic_dependencies(
        std::size_t                 injection_index,
        const DependencyChainNode&  dependency_chain,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;
};

}   // namespace ddge::app::v2

namespace ddge::app::v2 {

template <auto injection_T>
struct ErasedEntryInjectionLambda {
    static auto operator()(EntryBuilderContainer& builders, Registry& registry) -> void
    {
        helper(injection_T, builders, registry);
    }

    template <typename... Dependencies_T, typename Builder_T>
    static auto helper(
        auto (*)(Dependencies_T...)->Builder_T,
        EntryBuilderContainer& builders,
        Registry&              registry
    ) -> void
    {
        builders.insert(
            std::invoke(
                injection_T, fetch_dependency<Dependencies_T>(builders, registry)...
            )
        );
    }

    template <typename Dependency_T>
    // ReSharper disable once CppNotAllPathsReturnValue
    static auto fetch_dependency(EntryBuilderContainer& builders, Registry& registry)
        -> Dependency_T
    {
        using StrippedDependency = strip_dependency_t<Dependency_T>;

        if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>) {
            if constexpr (util::meta::specialization_of_c<Dependency_T, util::OptionalRef>)
            {
                return builders.find<StrippedDependency>();
            }
            else {
                return builders.at<StrippedDependency>();
            }
        }
        else if constexpr (std::derived_from<StrippedDependency, ConfigurationEntry>) {
            if constexpr (util::meta::specialization_of_c<Dependency_T, util::OptionalRef>)
            {
                return registry.find<StrippedDependency>();
            }
            else {
                return registry.at<StrippedDependency>();
            }
        }
        else {
            static_assert(false, "invalid dependency");
        }
    }
};

template <util::meta::function_c Injection_T>
auto EntryInjectionContainer::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_builder_hashes, util::meta::hash<util::meta::result_of_t<Injection_T>>()
    );
}

template <auto injection_T>
auto EntryInjectionContainer::try_insert() -> bool
{
    using Builder = util::meta::result_of_t<decltype(injection_T)>;

    if (contains<std::remove_pointer_t<decltype(injection_T)>>()) {
        return false;
    }

    m_injections.emplace_back(ErasedEntryInjectionLambda<injection_T>{});
    m_builder_hashes.push_back(util::meta::hash<Builder>());
    [this]<typename... Dependencies_T>(util::TypeList<Dependencies_T...>) -> void {
        std::pmr::vector<uint64_t>& dependencies = m_dependency_hashes.emplace_back();
        dependencies.append_range(
            std::initializer_list{
                util::meta::hash<strip_dependency_t<Dependencies_T>>()...,
            }
        );
    }(util::meta::arguments_of_t<decltype(injection_T)>{});
    m_dependent_builder_hashes.emplace_back();

#ifdef ENGINE_DEBUG
    m_builder_names.emplace_back(util::meta::name_of<Builder>());
#endif

    return true;
}

}   // namespace ddge::app::v2
