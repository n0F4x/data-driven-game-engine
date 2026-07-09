module;

#include <concepts>
#include <memory_resource>

export module ddge.registry.RegistryBuilder;

import ddge.registry.BuildableEntryBase;
import ddge.registry.BuildDirector;
import ddge.registry.entry_c;
import ddge.registry.EntryBuilderContainer;
import ddge.registry.EntryInjectionContainer;
import ddge.registry.Registry;

namespace ddge::registry {

export class RegistryBuilder {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    RegistryBuilder(const RegistryBuilder&)     = delete;
    RegistryBuilder(RegistryBuilder&&) noexcept = default;
    RegistryBuilder(RegistryBuilder&&, const allocator_type&);

    explicit RegistryBuilder() = default;
    explicit RegistryBuilder(const allocator_type&);

    template <decays_to_entry_c... Entries_T>
    explicit RegistryBuilder(
        std::allocator_arg_t,
        const allocator_type&,
        Entries_T&&... entries
    );

    auto operator=(const RegistryBuilder&) -> RegistryBuilder& = delete;
    auto operator=(RegistryBuilder&&) -> RegistryBuilder&      = default;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <entry_c Entry_T>
    auto register_entry() -> void;

    [[nodiscard]]
    auto build(std::pmr::memory_resource& transient_memory_resource) && -> Registry;

private:
    EntryInjectionContainer m_injections;
    EntryBuilderContainer   m_builders;
    Registry                m_registry;
};

}   // namespace ddge::registry

namespace ddge::registry {

template <typename Entry_T>
concept buildable_entry_c = std::derived_from<Entry_T, internal::BuildableEntryBase>;

template <decays_to_entry_c... Entries_T>
RegistryBuilder::RegistryBuilder(
    std::allocator_arg_t,
    const allocator_type& allocator,
    Entries_T&&... entries
)
    : RegistryBuilder{ allocator }
{
    (m_registry.insert(std::forward<Entries_T>(entries)), ...);
}

template <entry_c Entry_T>
auto RegistryBuilder::register_entry() -> void
{
    if constexpr (buildable_entry_c<Entry_T>)
    {
        BuildDirector<Entry_T> build_director{
            m_injections,
            m_builders,
            m_registry,
        };

        describe_build(build_director);
    }
    else
    {
        m_registry.try_emplace<Entry_T>();
    }
}

}   // namespace ddge::registry
