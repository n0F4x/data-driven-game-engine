module;

#include <functional>

export module ddge.modules.ecs:ArchetypeID;

import ddge.util.containers.Strong;

import :Archetype;
import :ComponentID;

class ArchetypeID {
public:
    constexpr explicit ArchetypeID(const Archetype& archetype) noexcept
        : m_archetype_ref{ archetype }
    {}

    [[nodiscard]]
    constexpr auto operator*() const -> const Archetype&
    {
        return m_archetype_ref;
    }

    [[nodiscard]]
    constexpr auto operator->() const -> const Archetype*
    {
        return &m_archetype_ref.get();
    }

    [[nodiscard]]
    auto operator==(const ArchetypeID& other) const -> bool
    {
        return m_archetype_ref.get() == other.m_archetype_ref.get();
    }

    [[nodiscard]]
    auto operator<=>(const ArchetypeID& other) const
    {
        return m_archetype_ref.get() == other.m_archetype_ref.get();
    }

    [[nodiscard]]
    constexpr auto get() const noexcept -> const Archetype&
    {
        return m_archetype_ref.get();
    }

private:
    friend struct ArchetypeIDHashClosure;

    std::reference_wrapper<const Archetype> m_archetype_ref;
};

template <>
struct std::hash<ArchetypeID> {
    [[nodiscard]]
    constexpr static auto operator()(const ArchetypeID archetype_id) noexcept
        -> std::size_t
    {
        return std::hash<Archetype>{}(*archetype_id);
    }
};
