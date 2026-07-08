module;

#include <algorithm>
#include <format>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include "ddge/util/contract_macros.hpp"

module ddge.app.v2.registry.EntryBuilderContainer;

import ddge.app.v2.registry.CyclicDependencyDetected;
import ddge.app.v2.registry.DependencyChainNode;
import ddge.app.v2.registry.ReverseDependencyChainNode;
import ddge.util.contracts;

namespace ddge::app::v2 {

EntryBuilderContainer::EntryBuilderContainer(
    EntryBuilderContainer&& other,
    const allocator_type&   allocator
)
    : m_builders{ std::move(other.m_builders), allocator },
      m_entry_hashes{ std::move(other.m_entry_hashes), allocator },
      m_builder_dependency_entry_hashes{
          std::move(other.m_builder_dependency_entry_hashes),
          allocator,
      },
      m_dependent_builder_entry_hashes{
          std::move(other.m_dependent_builder_entry_hashes),
          allocator,
      },
      m_entry_dependency_hashes{ std::move(other.m_entry_dependency_hashes), allocator },
      m_dependent_entry_hashes{ std::move(other.m_dependent_entry_hashes), allocator }
#ifdef ENGINE_DEBUG
      ,
      m_entry_names{ std::move(other.m_entry_names), allocator },
      m_builder_names{ std::move(other.m_builder_names), allocator }
#endif
{
}

EntryBuilderContainer::EntryBuilderContainer(const allocator_type& allocator)
    : m_builders{ allocator },
      m_entry_hashes{ allocator },
      m_builder_dependency_entry_hashes{ allocator },
      m_dependent_builder_entry_hashes{ allocator },
      m_entry_dependency_hashes{ allocator },
      m_dependent_entry_hashes{ allocator }
#ifdef ENGINE_DEBUG
      ,
      m_entry_names{ allocator },
      m_builder_names{ allocator }
#endif
{
}

auto EntryBuilderContainer::get_allocator() const noexcept -> allocator_type
{
    return m_builders.get_allocator();
}

auto EntryBuilderContainer::build(
    Registry&                                   registry,
    [[maybe_unused]] std::pmr::memory_resource& transient_memory_resource
) && -> void
{
    PRECOND(!check_cyclic_dependencies(transient_memory_resource));

    sort();

    for (ErasedEntryBuilder& builder : m_builders)
    {
        std::move(builder)(*this, registry);
    }
}

auto EntryBuilderContainer::try_index_of_builder(const uint64_t hash) const noexcept
    -> std::optional<std::size_t>
{
    const auto hash_iter{ std::ranges::find(m_entry_hashes, hash) };
    if (hash_iter == m_entry_hashes.cend())
    {
        return std::nullopt;
    }
    return static_cast<std::size_t>(std::distance(m_entry_hashes.begin(), hash_iter));
}

auto EntryBuilderContainer::sort() -> void
{
    /*
     * Sorting requires that there is no cyclic dependency
     */
    sort_based_on_entry_dependencies();
    sort_based_on_builder_dependencies();
}

#ifdef ENGINE_DEBUG
auto EntryBuilderContainer::check_cyclic_dependencies(
    std::pmr::memory_resource& transient_memory_resource
) const -> bool
{
    for (const auto& [builder_index, entry_hash] : std::views::enumerate(m_entry_hashes))
    {
        std::pmr::vector<uint64_t> reversed_hash_cache{ &transient_memory_resource };
        reversed_hash_cache.push_back(entry_hash);

        if (check_builder_cyclic_dependencies(
                static_cast<std::size_t>(builder_index),
                reversed_hash_cache,
                transient_memory_resource
            ))
        {
            return true;
        }

        std::pmr::vector<uint64_t> hash_cache{ &transient_memory_resource };
        hash_cache.push_back(entry_hash);

        if (check_entry_cyclic_dependencies(
                static_cast<std::size_t>(builder_index),
                reversed_hash_cache,
                hash_cache,
                transient_memory_resource
            ))
        {
            return true;
        }
    }

    return false;
}
#endif

auto EntryBuilderContainer::sort_based_on_builder_dependencies() -> void
{
    collect_dependent_builder_hashes();

    bool done_sorting{};
    while (!done_sorting)
    {
        done_sorting = true;
        for (auto&& [index, hash, dependent_hashes] : std::views::zip(
                 std::views::iota(0uz),
                 m_entry_hashes,
                 m_dependent_builder_entry_hashes
             ))
        {
            if (!dependent_hashes.has_value())
            {
                continue;
            }

            if (dependent_hashes->empty())
            {
                for (auto& dependent_dependency_hashes : m_dependent_builder_entry_hashes)
                {
                    if (dependent_dependency_hashes.has_value())
                    {
                        std::erase(*dependent_dependency_hashes, hash);
                    }
                }

                dependent_hashes.reset();

                /*
                 * Pushing down invalidates iterators
                 */
                push_down_builder_dependencies_of(index);
            }
            else
            {
                done_sorting = false;
            }
        }
    }
}

auto EntryBuilderContainer::sort_based_on_entry_dependencies() -> void
{
    collect_dependent_entry_hashes();

    bool done_sorting{};
    while (!done_sorting)
    {
        done_sorting = true;
        for (auto&& [index, hash, dependent_hashes] : std::views::reverse(
                 std::views::zip(
                     std::views::iota(0uz),
                     m_entry_hashes,
                     m_dependent_entry_hashes
                 )
             ))
        {
            if (!dependent_hashes.has_value())
            {
                continue;
            }

            if (dependent_hashes->empty())
            {
                for (auto& dependent_dependency_hashes : m_dependent_entry_hashes)
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
                bubble_up_entry_dependencies_of(index);
            }
            else
            {
                done_sorting = false;
            }
        }
    }
}

auto EntryBuilderContainer::collect_dependent_builder_hashes() -> void
{
    for (auto&& [hash, dependent_builder_hashes] :
         std::views::zip(m_entry_hashes, m_dependent_builder_entry_hashes))
    {
        if (dependent_builder_hashes.has_value())
        {
            dependent_builder_hashes->clear();
        }
        else
        {
            dependent_builder_hashes.emplace();
        }

        for (const auto& [dependent_hash, builder_dependency_hashes] :
             std::views::zip(m_entry_hashes, m_builder_dependency_entry_hashes))
        {
            if (std::ranges::contains(builder_dependency_hashes, hash))
            {
                dependent_builder_hashes->push_back(dependent_hash);
            }
        }
    }
}

auto EntryBuilderContainer::collect_dependent_entry_hashes() -> void
{
    for (auto&& [hash, dependent_entry_hashes] :
         std::views::zip(m_entry_hashes, m_dependent_entry_hashes))
    {
        if (dependent_entry_hashes.has_value())
        {
            dependent_entry_hashes->clear();
        }
        else
        {
            dependent_entry_hashes.emplace();
        }

        for (const auto& [dependent_hash, entry_dependency_hashes] :
             std::views::zip(m_entry_hashes, m_entry_dependency_hashes))
        {
            if (std::ranges::contains(entry_dependency_hashes, hash))
            {
                dependent_entry_hashes->push_back(dependent_hash);
            }
        }
    }
}

auto EntryBuilderContainer::push_down_builder_dependencies_of(const std::size_t index)
    -> void
{
    const auto zipped_containers{
        std::views::zip(
            m_builders,
            m_entry_hashes,
            m_builder_dependency_entry_hashes,
            m_dependent_builder_entry_hashes,
            m_entry_dependency_hashes,
            m_dependent_entry_hashes
#ifdef ENGINE_DEBUG
            ,
            m_entry_names,
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

    for (const std::span dependency_hashes{ m_builder_dependency_entry_hashes[index] };
         const uint64_t  dependency_hash : dependency_hashes)
    {
        const auto hash_iter{
            std::next(
                m_entry_hashes.begin(),
                std::distance(zipped_containers.begin(), iter)
            ),
        };
        const auto dependency_hash_iter{
            std::ranges::find(m_entry_hashes.begin(), hash_iter, dependency_hash),
        };
        if (dependency_hash_iter == hash_iter)
        {
            continue;
        }

        const auto dependency_iter{
            std::next(
                zipped_containers.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)
            ),
        };

        iter = std::prev(
            std::ranges::rotate(
                dependency_iter,
                std::next(dependency_iter),
                std::next(iter)
            )
                .begin()
        );
    }
}

auto EntryBuilderContainer::bubble_up_entry_dependencies_of(const std::size_t index)
    -> void
{
    const auto zipped_containers{
        std::views::zip(
            m_builders,
            m_entry_hashes,
            m_builder_dependency_entry_hashes,
            m_dependent_builder_entry_hashes,
            m_entry_dependency_hashes,
            m_dependent_entry_hashes
#ifdef ENGINE_DEBUG
            ,
            m_entry_names,
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

    for (const std::span dependency_hashes{ m_entry_dependency_hashes[index] };
         const uint64_t  dependency_hash : dependency_hashes)
    {
        const auto dependency_hash_iter{
            std::ranges::find(
                std::next(
                    m_entry_hashes.begin(),
                    std::distance(zipped_containers.begin(), std::next(iter))
                ),
                m_entry_hashes.end(),
                dependency_hash
            ),
        };
        if (dependency_hash_iter == m_entry_hashes.cend())
        {
            continue;
        }

        const auto dependency_iter{
            std::next(
                zipped_containers.begin(),
                std::distance(m_entry_hashes.begin(), dependency_hash_iter)
            ),
        };

        iter = std::ranges::rotate(iter, dependency_iter, std::next(dependency_iter))
                   .begin();
    }
}

#ifdef ENGINE_DEBUG
auto EntryBuilderContainer::check_entry_cyclic_dependencies(
    const std::size_t                 builder_index,
    const std::pmr::vector<uint64_t>& reversed_hash_cache,
    std::pmr::vector<uint64_t>&       hash_cache,
    std::pmr::memory_resource&        transient_memory_resource
) const -> bool
{
    const DependencyChainNode dependency_chain_node{
        .hash = m_entry_hashes[builder_index],
        .name = m_entry_names[builder_index],
    };
    for (const uint64_t dependency_hash : m_entry_dependency_hashes[builder_index])
    {
        if (const auto dependency_hash_iter
            = std::ranges::find(m_entry_hashes, dependency_hash);
            dependency_hash_iter != m_entry_hashes.cend())
        {
            if (check_entry_cyclic_dependencies(
                    static_cast<std::size_t>(
                        std::distance(m_entry_hashes.begin(), dependency_hash_iter)
                    ),
                    dependency_chain_node,
                    hash_cache,
                    builder_index,
                    reversed_hash_cache,
                    transient_memory_resource
                ))
            {
                return true;
            }
        }
    }

    return false;
}

auto EntryBuilderContainer::check_entry_cyclic_dependencies(
    const std::size_t                 builder_index,
    const DependencyChainNode&        dependency_chain,
    std::pmr::vector<uint64_t>&       hash_cache,
    const std::size_t                 root_builder_index,
    const std::pmr::vector<uint64_t>& reversed_hash_cache,
    std::pmr::memory_resource&        transient_memory_resource
) const -> bool
{
    const uint64_t         entry_hash{ m_entry_hashes[builder_index] };
    const std::string_view entry_name{ m_entry_names[builder_index] };

    if (dependency_chain.contains(entry_hash))
    {
        PRECOND_AS(
            false,
            CyclicDependencyDetected,
            std::format(
                "Cyclic dependency detected - entry of type `{}` depends on itself "   //
                "({} -> {})",
                entry_name,
                dependency_chain.format(&transient_memory_resource),
                entry_name
            )
        );
        return true;
    }

    if (std::ranges::binary_search(reversed_hash_cache, entry_hash))
    {
        PRECOND_AS(
            false,
            CyclicDependencyDetected,
            std::format(
                "Cyclic dependency detected"
                " - entry of type `{}` depends on both an entry of type `{}` ({} -> {})"
                " and its builder of type `{}` ({})",
                m_entry_names[root_builder_index],
                entry_name,
                dependency_chain.format(&transient_memory_resource),
                entry_name,
                m_builder_names[builder_index],
                path_as_string_to_builder_from(
                    root_builder_index,
                    entry_hash,
                    &transient_memory_resource,
                    transient_memory_resource
                )
            )
        );
        return true;
    }

    if (std::ranges::binary_search(hash_cache, entry_hash))
    {
        return false;
    }
    hash_cache.insert(std::ranges::lower_bound(hash_cache, entry_hash), entry_hash);

    const DependencyChainNode dependency_chain_node{
        .previous = &dependency_chain,
        .hash     = entry_hash,
        .name     = entry_name,
    };

    for (const uint64_t dependency_hash : m_entry_dependency_hashes[builder_index])
    {
        if (const auto dependency_hash_iter
            = std::ranges::find(m_entry_hashes, dependency_hash);
            dependency_hash_iter != m_entry_hashes.cend())
        {
            if (check_entry_cyclic_dependencies(
                    static_cast<std::size_t>(
                        std::distance(m_entry_hashes.begin(), dependency_hash_iter)
                    ),
                    dependency_chain_node,
                    hash_cache,
                    root_builder_index,
                    reversed_hash_cache,
                    transient_memory_resource
                ))
            {
                return true;
            }
        }
    }

    return false;
}

auto EntryBuilderContainer::check_builder_cyclic_dependencies(
    const std::size_t           builder_index,
    std::pmr::vector<uint64_t>& hash_cache,
    std::pmr::memory_resource&  transient_memory_resource
) const -> bool
{
    const std::pmr::string build_method_name{
        m_builder_names[builder_index] + "::build()",
        &transient_memory_resource,
    };

    const ReverseDependencyChainNode reversed_dependency_chain_node{
        .hash = m_entry_hashes[builder_index],
        .name = build_method_name,
    };

    for (const uint64_t dependency_hash :
         m_builder_dependency_entry_hashes[builder_index])
    {
        if (const auto dependency_hash_iter
            = std::ranges::find(m_entry_hashes, dependency_hash);
            dependency_hash_iter != m_entry_hashes.cend())
        {
            if (check_builder_cyclic_dependencies(
                    static_cast<std::size_t>(
                        std::distance(m_entry_hashes.begin(), dependency_hash_iter)
                    ),
                    reversed_dependency_chain_node,
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

auto EntryBuilderContainer::check_builder_cyclic_dependencies(
    const std::size_t                 builder_index,
    const ReverseDependencyChainNode& dependency_chain,
    std::pmr::vector<uint64_t>&       hash_cache,
    std::pmr::memory_resource&        transient_memory_resource
) const -> bool
{
    const uint64_t         entry_hash{ m_entry_hashes[builder_index] };
    const std::pmr::string build_method_name{
        m_builder_names[builder_index] + "::build()",
        &transient_memory_resource,
    };

    if (dependency_chain.contains(entry_hash))
    {
        PRECOND_AS(
            false,
            CyclicDependencyDetected,
            std::format(
                "Cyclic dependency detected - entry builder of type `{}` depends on "
                "itself "   //
                "({} <- {})",
                m_builder_names[builder_index],
                dependency_chain.format(&transient_memory_resource),
                build_method_name
            )
        );
        return true;
    }

    if (std::ranges::binary_search(hash_cache, entry_hash))
    {
        return false;
    }
    hash_cache.insert(std::ranges::lower_bound(hash_cache, entry_hash), entry_hash);

    const ReverseDependencyChainNode dependency_chain_node{
        .previous = &dependency_chain,
        .hash     = entry_hash,
        .name     = build_method_name,
    };

    for (const uint64_t dependency_hash :
         m_builder_dependency_entry_hashes[builder_index])
    {
        if (const auto dependency_hash_iter
            = std::ranges::find(m_entry_hashes, dependency_hash);
            dependency_hash_iter != m_entry_hashes.cend())
        {
            if (check_builder_cyclic_dependencies(
                    static_cast<std::size_t>(
                        std::distance(m_entry_hashes.begin(), dependency_hash_iter)
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

auto EntryBuilderContainer::path_as_string_to_builder_from(
    const std::size_t                        source_builder_index,
    const uint64_t                           destination_builder_entry_hash,
    const std::pmr::polymorphic_allocator<>& allocator,
    std::pmr::memory_resource&               transient_memory_resource
) const -> std::pmr::string
{
    std::pmr::vector<uint64_t> hash_cache{ &transient_memory_resource };
    hash_cache.push_back(m_entry_hashes[source_builder_index]);

    const DependencyChainNode dependency_chain_node{
        .hash = m_entry_hashes[source_builder_index],
        .name = m_entry_names[source_builder_index],
    };

    for (const uint64_t builder_dependency_entry_hash :
         m_builder_dependency_entry_hashes[source_builder_index])
    {
        if (auto result{
                path_as_string_to_builder_from(
                    builder_dependency_entry_hash,
                    destination_builder_entry_hash,
                    dependency_chain_node,
                    hash_cache,
                    allocator
                ),
            };
            result.has_value())
        {
            return std::move(*result);
        }
    }

    std::unreachable();
}

auto EntryBuilderContainer::path_as_string_to_builder_from(
    const uint64_t                           builder_entry_hash,
    const uint64_t                           destination_builder_entry_hash,
    const DependencyChainNode&               dependency_chain,
    std::pmr::vector<uint64_t>&              hash_cache,
    const std::pmr::polymorphic_allocator<>& allocator
) const -> std::optional<std::pmr::string>
{
    if (std::ranges::binary_search(hash_cache, builder_entry_hash))
    {
        return std::nullopt;
    }

    const std::size_t builder_index{
        static_cast<std::size_t>(
            std::distance(
                m_entry_hashes.begin(),
                std::ranges::find(m_entry_hashes, builder_entry_hash)
            )   //
        ),
    };

    const DependencyChainNode dependency_chain_node{
        .previous = &dependency_chain,
        .hash     = builder_entry_hash,
        .name     = m_builder_names[builder_index],
    };

    if (builder_entry_hash == destination_builder_entry_hash)
    {
        return dependency_chain_node.format(allocator);
    }

    hash_cache.insert(
        std::ranges::lower_bound(hash_cache, builder_entry_hash),
        builder_entry_hash
    );

    for (const uint64_t builder_dependency_entry_hash :
         m_builder_dependency_entry_hashes[builder_index])
    {
        if (auto result{
                path_as_string_to_builder_from(
                    builder_dependency_entry_hash,
                    destination_builder_entry_hash,
                    dependency_chain_node,
                    hash_cache,
                    allocator
                ),
            };
            result.has_value())
        {
            return std::move(*result);
        }
    }

    return std::nullopt;
}
#endif

}   // namespace ddge::app::v2
