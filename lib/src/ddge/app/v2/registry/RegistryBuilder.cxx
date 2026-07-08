module;

#include <memory_resource>
#include <utility>

module ddge.app.v2.registry.RegistryBuilder;

import ddge.app.v2.registry.EntryBuilderContainer;
import ddge.app.v2.registry.EntryInjectionContainer;

ddge::app::v2::RegistryBuilder::RegistryBuilder(
    RegistryBuilder&&     other,
    const allocator_type& allocator
)
    : m_injections{ std::move(other.m_injections), allocator },
      m_builders{ std::move(other.m_builders), allocator },
      m_registry{ std::move(other.m_registry), allocator }
{
}

ddge::app::v2::RegistryBuilder::RegistryBuilder(const allocator_type& allocator)
    : m_injections{ allocator },
      m_builders{ allocator },
      m_registry{ allocator }
{
}

auto ddge::app::v2::RegistryBuilder::get_allocator() const noexcept -> allocator_type
{
    return m_injections.get_allocator();
}

auto ddge::app::v2::RegistryBuilder::build(
    std::pmr::memory_resource& transient_memory_resource
) && -> Registry
{
    std::move(m_injections).build(m_builders, m_registry, transient_memory_resource);
    std::move(m_builders).build(m_registry, transient_memory_resource);

    return std::move(m_registry);
}
