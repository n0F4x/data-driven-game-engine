module;

#include <functional>

export module core.ecs:ArchetypeID;

import utility.Strong;

import :ComponentID;
import :ArchetypeInfo;

class ArchetypeID {
public:
    constexpr explicit ArchetypeID(const ArchetypeInfo& archetype)
        : m_archetype_ref{ archetype }
    {}

    auto operator==(const ArchetypeID&) const -> bool = default;
    auto operator<=>(const ArchetypeID&) const        = default;

    [[nodiscard]]
    constexpr auto get() const noexcept -> const ArchetypeInfo&
    {
        return m_archetype_ref;
    }

private:
    friend struct ArchetypeIDHashAdaptorClosure;

    std::reference_wrapper<const ArchetypeInfo> m_archetype_ref;
};

struct ArchetypeIDHashAdaptorClosure {
    [[nodiscard]]
    constexpr static auto operator()(const ArchetypeID archetype_id) noexcept -> size_t
    {
        return std::hash<ArchetypeInfo>{}(archetype_id.m_archetype_ref.get());
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
