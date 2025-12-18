export module ddge.modules.config.engine_name;

import ddge.utility.containers.StringLiteral;

namespace ddge::config {

export [[nodiscard]]
constexpr auto engine_name() noexcept -> util::StringLiteral;

}   // namespace ddge::config

module :private;

constexpr auto ddge::config::engine_name() noexcept -> util::StringLiteral
{
    constexpr static util::StringLiteral result{ "ENGINE" };
    return result;
}
