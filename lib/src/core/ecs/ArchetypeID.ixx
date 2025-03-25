module;

#include <functional>

export module core.ecs:ArchetypeID;

import utility.Strong;

import :Archetype;
import :ComponentID;

class ArchetypeID {
public:
    constexpr explicit ArchetypeID(const Archetype& archetype)
        : m_archetype_ref{ archetype }
    {}

    auto operator==(const ArchetypeID&) const -> bool = default;
    auto operator<=>(const ArchetypeID&) const        = default;

    [[nodiscard]]
    constexpr auto get() const noexcept -> const Archetype&
    {
        return m_archetype_ref;
    }

private:
    friend struct ArchetypeIDHashAdaptorClosure;

    std::reference_wrapper<const Archetype> m_archetype_ref;
};

struct ArchetypeIDHashAdaptorClosure {
    [[nodiscard]]
    constexpr static auto operator()(const ArchetypeID archetype_id) noexcept -> size_t
    {
        return std::hash<Archetype>{}(archetype_id.m_archetype_ref.get());
    }
};

template <>
struct std::hash<ArchetypeID> {
    [[nodiscard]]
    constexpr static auto operator()(const ArchetypeID archetype_id) noexcept
        -> std::size_t
    {
        return ArchetypeIDHashAdaptorClosure::operator()(archetype_id);
    }
};
