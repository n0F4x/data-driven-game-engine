module;

#include <string>
#include <string_view>

export module ddge.app.v2.config.Config;

import ddge.app.v2.registry.ConfigurationEntry;
import ddge.config.engine_name;
import ddge.config.engine_version;
import ddge.config.Version;
import ddge.util.StringLiteral;

namespace ddge::app::v2 {

export class Config : public ConfigurationEntry {
public:
    constexpr explicit Config(
        const std::string_view app_name    = "",
        const config::Version& app_version = {}
    )
        : m_app_name{ app_name },
          m_app_version{ app_version }
    {}

    [[nodiscard]]
    consteval static auto engine_name() noexcept -> util::StringLiteral
    {
        return config::engine_name();
    }

    [[nodiscard]]
    constexpr static auto engine_version() noexcept -> const config::Version&
    {
        return config::engine_version();
    }

    [[nodiscard]]
    constexpr auto app_name() const noexcept -> const std::string&
    {
        return m_app_name;
    }

    [[nodiscard]]
    constexpr auto app_version() const noexcept -> const config::Version&
    {
        return m_app_version;
    }

private:
    std::string     m_app_name;
    config::Version m_app_version{};
};

}   // namespace ddge::app::v2
