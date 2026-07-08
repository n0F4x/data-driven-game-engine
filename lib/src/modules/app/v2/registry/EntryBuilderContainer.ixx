module;

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "util/contract_macros.hpp"

export module ddge.modules.app.v2.registry.EntryBuilderContainer;

import ddge.modules.app.v2.registry.DependencyChainNode;
import ddge.modules.app.v2.registry.EntryBase;
import ddge.modules.app.v2.registry.EntryBuilderBase;
import ddge.modules.app.v2.registry.strip_dependency_t;
import ddge.modules.app.v2.registry.Registry;
import ddge.modules.app.v2.registry.ReverseDependencyChainNode;
import ddge.util.containers.OptionalRef;
import ddge.util.containers.MoveOnlyFunction;
import ddge.util.contracts;
import ddge.util.meta.algorithms.for_each;
import ddge.util.meta.concepts.specialization_of;
import ddge.util.meta.reflection;
import ddge.util.meta.type_traits.const_like;
import ddge.util.meta.type_traits.forward_like;
import ddge.util.meta.type_traits.functional.arguments_of;
import ddge.util.meta.type_traits.functional.result_of;
import ddge.util.TypeList;

namespace ddge::app::v2 {

export class EntryBuilderContainer;

using ErasedEntryBuilder =
    util::MoveOnlyFunction<auto(EntryBuilderContainer&, Registry&) &&->void, 0>;

export class EntryBuilderContainer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    EntryBuilderContainer(EntryBuilderContainer&&, const allocator_type&);

    explicit EntryBuilderContainer() = default;
    explicit EntryBuilderContainer(const allocator_type&);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <typename Builder_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

    template <typename Builder_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::meta::const_like_t<Builder_T, Self_T>>;

    template <typename Builder_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) noexcept -> util::meta::const_like_t<Builder_T, Self_T>&;


    template <typename Builder_T>
    auto insert(Builder_T&& builder) -> void;

    template <typename Builder_T, typename... Args_T>
        requires std::constructible_from<Builder_T, Args_T&&...>
    auto try_emplace(Args_T&&... args) -> bool;

    auto build(
        Registry&                  registry,
        std::pmr::memory_resource& transient_memory_resource
    ) && -> void;

private:
    std::pmr::vector<ErasedEntryBuilder>         m_builders;
    std::pmr::vector<uint64_t>                   m_entry_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_builder_dependency_entry_hashes;
    std::pmr::vector<std::optional<std::pmr::vector<uint64_t>>>
                                                 m_dependent_builder_entry_hashes;
    std::pmr::vector<std::pmr::vector<uint64_t>> m_entry_dependency_hashes;
    std::pmr::vector<std::optional<std::pmr::vector<uint64_t>>> m_dependent_entry_hashes;
#ifdef ENGINE_DEBUG
    std::pmr::vector<std::pmr::string> m_entry_names;
    std::pmr::vector<std::pmr::string> m_builder_names;
#endif


    [[nodiscard]]
    auto try_index_of_builder(uint64_t hash) const noexcept -> std::optional<std::size_t>;

    auto sort() -> void;

    [[nodiscard]]
    auto check_cyclic_dependencies(
        std::pmr::memory_resource& transient_memory_resource
    ) const -> bool;

    auto sort_based_on_builder_dependencies() -> void;
    auto sort_based_on_entry_dependencies() -> void;

    auto collect_dependent_builder_hashes() -> void;
    auto collect_dependent_entry_hashes() -> void;

    auto push_down_builder_dependencies_of(std::size_t index) -> void;
    auto bubble_up_entry_dependencies_of(std::size_t index) -> void;

    [[nodiscard]]
    auto check_entry_cyclic_dependencies(
        std::size_t                       builder_index,
        const std::pmr::vector<uint64_t>& reversed_hash_cache,
        std::pmr::vector<uint64_t>&       hash_cache,
        std::pmr::memory_resource&        transient_memory_resource
    ) const -> bool;
    [[nodiscard]]
    auto check_entry_cyclic_dependencies(
        std::size_t                       builder_index,
        const DependencyChainNode&        dependency_chain,
        std::pmr::vector<uint64_t>&       hash_cache,
        std::size_t                       root_builder_index,
        const std::pmr::vector<uint64_t>& reversed_hash_cache,
        std::pmr::memory_resource&        transient_memory_resource
    ) const -> bool;
    [[nodiscard]]
    auto check_builder_cyclic_dependencies(
        std::size_t                 builder_index,
        std::pmr::vector<uint64_t>& hash_cache,
        std::pmr::memory_resource&  transient_memory_resource
    ) const -> bool;
    [[nodiscard]]
    auto check_builder_cyclic_dependencies(
        std::size_t                       builder_index,
        const ReverseDependencyChainNode& dependency_chain,
        std::pmr::vector<uint64_t>&       hash_cache,
        std::pmr::memory_resource&        transient_memory_resource
    ) const -> bool;

    [[nodiscard]]
    auto path_as_string_to_builder_from(
        std::size_t                              source_builder_index,
        uint64_t                                 destination_builder_entry_hash,
        const std::pmr::polymorphic_allocator<>& allocator,
        std::pmr::memory_resource&               transient_memory_resource
    ) const -> std::pmr::string;
    [[nodiscard]]
    auto path_as_string_to_builder_from(
        uint64_t                                 builder_entry_hash,
        uint64_t                                 destination_builder_entry_hash,
        const DependencyChainNode&               dependency_chain,
        std::pmr::vector<uint64_t>&              hash_cache,
        const std::pmr::polymorphic_allocator<>& allocator
    ) const -> std::optional<std::pmr::string>;
};

}   // namespace ddge::app::v2

namespace ddge::app::v2 {

template <typename Dependency_T>
// ReSharper disable once CppNotAllPathsReturnValue
auto fetch_dependency(EntryBuilderContainer& builders, Registry& registry) -> Dependency_T
{
    using StrippedDependency = strip_dependency_t<Dependency_T>;

    if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>) {
        if constexpr (util::meta::specialization_of_c<Dependency_T, util::OptionalRef>) {
            return builders.find<StrippedDependency>();
        }
        else {
            return builders.at<StrippedDependency>();
        }
    }
    else if constexpr (std::derived_from<StrippedDependency, EntryBase>) {
        if constexpr (util::meta::specialization_of_c<Dependency_T, util::OptionalRef>) {
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

template <typename Builder_T>
struct ErasedEntryBuilderLambda {
    Builder_T builder;

    auto operator()(EntryBuilderContainer& builders, Registry& registry) && -> void
    {
        [this, &builders, &registry]<typename... Dependencies_T>(
            util::TypeList<Dependencies_T...>
        ) -> void {
            registry.insert(
                std::move(builder).build(
                    fetch_dependency<Dependencies_T>(builders, registry)...
                )
            );
        }(util::meta::arguments_of_t<decltype(&Builder_T::build)>{});
    }
};

template <typename Builder_T>
auto EntryBuilderContainer::contains() const noexcept -> bool
{
    return std::ranges::contains(
        m_entry_hashes,
        util::meta::hash<util::meta::result_of_t<decltype(&Builder_T::build)>>()
    );
}

template <typename Builder_T, typename Self_T>
auto EntryBuilderContainer::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Builder_T, Self_T>>
{
    const std::optional<std::size_t> builder_index{
        self.try_index_of_builder(
            util::meta::hash<util::meta::result_of_t<decltype(&Builder_T::build)>>()
        ),
    };
    if (!builder_index.has_value()) {
        return std::nullopt;
    }

    return util::any_cast<ErasedEntryBuilderLambda<Builder_T>>(
               self.m_builders[*builder_index]
    )
        .builder;
}

template <typename Builder_T, typename Self_T>
auto EntryBuilderContainer::at(this Self_T& self) noexcept
    -> util::meta::const_like_t<Builder_T, Self_T>&
{
    return *self.template find<Builder_T>();
}

template <typename Builder_T>
auto EntryBuilderContainer::insert(Builder_T&& builder) -> void
{
    [[maybe_unused]]
    const bool success =
        try_emplace<std::remove_cvref_t<Builder_T>>(std::forward<Builder_T>(builder));
    PRECOND(success);
}

template <typename Builder_T, typename... Args_T>
    requires std::constructible_from<Builder_T, Args_T&&...>
auto EntryBuilderContainer::try_emplace(Args_T&&... args) -> bool
{
    if (contains<Builder_T>()) {
        return false;
    }

    m_builders.emplace_back(
        std::in_place_type<ErasedEntryBuilderLambda<Builder_T>>,
        std::forward<Args_T>(args)...
    );
    m_entry_hashes.push_back(
        util::meta::hash<util::meta::result_of_t<decltype(&Builder_T::build)>>()
    );

    std::pmr::vector<uint64_t>& builder_dependency_hashes =
        m_builder_dependency_entry_hashes.emplace_back();
    builder_dependency_hashes.reserve(
        util::meta::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
    m_dependent_builder_entry_hashes.emplace_back();
    std::pmr::vector<uint64_t>& entry_dependency_hashes =
        m_entry_dependency_hashes.emplace_back();
    entry_dependency_hashes.reserve(
        util::meta::arguments_of_t<decltype(&Builder_T::build)>::size()
    );
    m_dependent_entry_hashes.emplace_back();
    util::meta::for_each<util::meta::arguments_of_t<decltype(&Builder_T::build)>>(
        [&builder_dependency_hashes,
         &entry_dependency_hashes]<typename Dependency_T> -> void {
            using StrippedDependency = strip_dependency_t<Dependency_T>;
            if constexpr (std::derived_from<StrippedDependency, EntryBuilderBase>) {
                builder_dependency_hashes.push_back(
                    util::meta::hash<
                        util::meta::result_of_t<decltype(&StrippedDependency::build)>>()
                );
            }
            else if constexpr (std::derived_from<StrippedDependency, EntryBase>) {
                entry_dependency_hashes.push_back(util::meta::hash<StrippedDependency>());
            }
            else {
                static_assert(false, "invalid build dependency");
            }
        }
    );

#ifdef ENGINE_DEBUG
    m_entry_names.emplace_back(
        util::meta::name_of<util::meta::result_of_t<decltype(&Builder_T::build)>>()
    );
    m_builder_names.emplace_back(util::meta::name_of<Builder_T>());
#endif

    return true;
}

}   // namespace ddge::app::v2
