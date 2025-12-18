module;

#include <cstdint>

export module ddge.modules.app.extensions.MetaInfo;

import ddge.utility.containers.StringLiteral;

namespace ddge::app {

inline namespace extensions {

export class MetaInfo {
public:
    constexpr MetaInfo(
        const util::StringLiteral application_name,
        const uint32_t            application_version
    )
        : m_application_name{ application_name },
          m_application_version{ application_version }
    {}

    [[nodiscard]]
    constexpr auto application_name() const noexcept -> util::StringLiteral
    {
        return m_application_name;
    }

    [[nodiscard]]
    constexpr auto application_version() const noexcept -> uint32_t
    {
        return m_application_version;
    }

private:
    util::StringLiteral m_application_name;
    uint32_t            m_application_version;
};

}   // namespace extensions

}   // namespace ddge::app
