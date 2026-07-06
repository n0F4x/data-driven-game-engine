module;

#include <algorithm>
#include <format>
#include <memory_resource>
#include <ranges>
#include <span>

#include "utility/contract_macros.hpp"

module ddge.modules.app.v2.registry.EntryInjectionContainer;

import ddge.modules.app.v2.registry.CyclicDependencyDetected;
import ddge.utility.contracts;

namespace ddge::app::v2 {

EntryInjectionContainer::EntryInjectionContainer(
    EntryInjectionContainer&& other,
    const allocator_type&     allocator
)
    : m_injections{ std::move(other.m_injections), allocator },
      m_builder_hashes{ std::move(other.m_builder_hashes), allocator },
      m_dependency_hashes{ std::move(other.m_dependency_hashes), allocator },
      m_dependent_builder_hashes{ std::move(other.m_dependent_builder_hashes), allocator }
#ifdef ENGINE_DEBUG
      ,
      m_builder_names{ std::move(other.m_builder_names), allocator }
#endif
{
}

EntryInjectionContainer::EntryInjectionContainer(const allocator_type& allocator)
    : m_injections{ allocator },
      m_builder_hashes{ allocator },
      m_dependency_hashes{ allocator },
      m_dependent_builder_hashes{ allocator }
#ifdef ENGINE_DEBUG
      ,
      m_builder_names{ allocator }
#endif
{
}

auto EntryInjectionContainer::get_allocator() const noexcept -> allocator_type
{
    return m_injections.get_allocator();
}

auto EntryInjectionContainer::build(
    EntryBuilderContainer&                      builders,
    Registry&                                   registry,
    [[maybe_unused]] std::pmr::memory_resource& transient_memory_resource
) && -> void
{
    PRECOND(!check_cyclic_dependencies(transient_memory_resource));

    sort();

    for (ErasedEntryInjection& injection : m_injections)
    {
        std::move(injection)(builders, registry);
    }
}

auto EntryInjectionContainer::sort() -> void
{
    /*
     * Sorting requires that there is no cyclic dependency
     */

    collect_dependent_builder_hashes();

    bool done_sorting{};
    while (!done_sorting)
    {
        done_sorting = true;
        for (auto&& [index, hash, dependent_hashes] : std::views::reverse(
                 std::views::zip(
                     std::views::iota(0uz),
                     m_builder_hashes,
                     m_dependent_builder_hashes
                 )
             ))
        {
            if (!dependent_hashes.has_value())
            {
                continue;
            }

            if (dependent_hashes->empty())
            {
                for (auto& dependent_dependency_hashes : m_dependent_builder_hashes)
                {
                    if (dependent_dependency_hashes.has_value())
                    {
                        std::erase(*dependent_dependency_hashes, hash);
                    }
                }

                dependent_hashes.reset();

                /*
                 * Bubbling up invalidates iterators
                 */
                bubble_up_dependencies_of(index);
            }
            else
            {
                done_sorting = false;
            }
        }
    }
}

#ifdef ENGINE_DEBUG
auto EntryInjectionContainer::check_cyclic_dependencies(
    std::pmr::memory_resource& transient_memory_resource
) const -> bool
{
    for (const auto& [builder_hash, dependency_hashes, builder_name] :
         std::views::zip(m_builder_hashes, m_dependency_hashes, m_builder_names))
    {
        std::pmr::vector<uint64_t> hash_cache{ &transient_memory_resource };
        hash_cache.push_back(builder_hash);

        const DependencyChainNode dependency_chain_node{
            .hash = builder_hash,
            .name = builder_name,
        };

        for (const uint64_t dependency_hash : dependency_hashes)
        {
            if (const auto dependency_hash_iter
                = std::ranges::find(m_builder_hashes, dependency_hash);
                dependency_hash_iter != m_builder_hashes.cend())
            {
                if (check_cyclic_dependencies(
                        static_cast<std::size_t>(
                            std::distance(m_builder_hashes.begin(), dependency_hash_iter)
                        ),
                        dependency_chain_node,
                        hash_cache,
                        transient_memory_resource
                    ))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
#endif

auto EntryInjectionContainer::collect_dependent_builder_hashes() -> void
{
    for (auto&& [builder_hash, dependent_builder_hashes] :
         std::views::zip(m_builder_hashes, m_dependent_builder_hashes))
    {
        if (dependent_builder_hashes.has_value())
        {
            dependent_builder_hashes->clear();
        }
        else
        {
            dependent_builder_hashes.emplace();
        }

        for (const auto& [dependent_hash, dependency_hashes] :
             std::views::zip(m_builder_hashes, m_dependency_hashes))
        {
            if (std::ranges::contains(dependency_hashes, builder_hash))
            {
                dependent_builder_hashes->push_back(dependent_hash);
            }
        }
    }
}

auto EntryInjectionContainer::bubble_up_dependencies_of(const std::size_t index) -> void
{
    const auto zipped_containers{
        std::views::zip(
            m_injections,
            m_builder_hashes,
            m_dependency_hashes,
            m_dependent_builder_hashes
#ifdef ENGINE_DEBUG
            ,
            m_builder_names
#endif
        ),
    };

    auto iter{
        std::next(
            zipped_containers.begin(),
            static_cast<std::iter_difference_t<decltype(zipped_containers.begin())>>(index)
        ),
    };

    for (const std::span dependency_hashes{ m_dependency_hashes[index] };
         const uint64_t  dependency_hash : dependency_hashes)
    {
        const auto dependency_hash_iter{
            std::ranges::find(
                std::next(
                    m_builder_hashes.begin(),
                    std::distance(zipped_containers.begin(), std::next(iter))
                ),
                m_builder_hashes.end(),
                dependency_hash
            ),
        };
        if (dependency_hash_iter == m_builder_hashes.cend())
        {
            continue;
        }

        const auto dependency_iter{
            std::next(
                zipped_containers.begin(),
                std::distance(m_builder_hashes.begin(), dependency_hash_iter)
            ),
        };

        iter = std::ranges::rotate(iter, dependency_iter, std::next(dependency_iter))
                   .begin();
    }
}

#ifdef ENGINE_DEBUG
auto EntryInjectionContainer::check_cyclic_dependencies(
    const std::size_t           injection_index,
    const DependencyChainNode&  dependency_chain,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) const -> bool
{
    const uint64_t         builder_hash{ m_builder_hashes[injection_index] };
    const std::string_view builder_name{ m_builder_names[injection_index] };

    if (dependency_chain.contains(builder_hash))
    {
        PRECOND_AS(
            false,
            CyclicDependencyDetected,
            std::format(
                "Cyclic dependency detected - entry builder of type `{}` depends on "
                "itself "   //
                "({} -> {})",
                builder_name,
                dependency_chain.format(&transient_memory_resource),
                builder_name
            )
        );
        return true;
    }

    if (std::ranges::binary_search(hash_cache, builder_hash))
    {
        return false;
    }
    hash_cache.insert(std::ranges::lower_bound(hash_cache, builder_hash), builder_hash);

    const DependencyChainNode dependency_chain_node{
        .previous = &dependency_chain,
        .hash     = builder_hash,
        .name     = builder_name,
    };

    for (const uint64_t dependency_hash : m_dependency_hashes[injection_index])
    {
        if (const auto dependency_hash_iter
            = std::ranges::find(m_builder_hashes, dependency_hash);
            dependency_hash_iter != m_builder_hashes.cend())
        {
            if (check_cyclic_dependencies(
                    static_cast<std::size_t>(
                        std::distance(m_builder_hashes.begin(), dependency_hash_iter)
                    ),
                    dependency_chain_node,
                    hash_cache,
                    transient_memory_resource
                ))
            {
                return true;
            }
        }
    }

    return false;
}
#endif

}   // namespace ddge::app::v2
