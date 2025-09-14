module;

#include <functional>

export module ddge.modules.ecs:ArchetypeID;

import ddge.utility.containers.Strong;

import :Archetype;
import :ComponentID;

class ArchetypeID {
public:
    constexpr explicit ArchetypeID(const Archetype& archetype)
        : m_archetype_ref{ archetype }
    {}

    [[nodiscard]]
    constexpr auto operator*() const -> const Archetype&
    {
        return m_archetype_ref.get();
    }

    constexpr auto operator->() const -> const Archetype*
    {
        return &m_archetype_ref.get();
    }

    auto operator==(const ArchetypeID&) const -> bool = default;
    auto operator<=>(const ArchetypeID&) const        = default;

    [[nodiscard]]
    constexpr auto get() const noexcept -> std::reference_wrapper<const Archetype>
    {
        return m_archetype_ref;
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
