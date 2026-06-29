module;

#include <functional>

export module ddge.modules.ecs:ArchetypeID;

import ddge.utility.containers.Strong;

import :Archetype;
import :ComponentID;

class ArchetypeID {
public:
    constexpr explicit ArchetypeID(const Archetype& archetype) noexcept
        : m_archetype{ &archetype }
    {}

    [[nodiscard]]
    constexpr auto operator*() const -> const Archetype&
    {
        return *m_archetype;
    }

    constexpr auto operator->() const -> const Archetype*
    {
        return m_archetype;
    }

    auto operator==(const ArchetypeID&) const -> bool = default;
    auto operator<=>(const ArchetypeID&) const        = default;

    [[nodiscard]]
    constexpr auto get() const noexcept -> const Archetype&
    {
        return *m_archetype;
    }

private:
    friend struct ArchetypeIDHashClosure;

    const Archetype* m_archetype;
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
