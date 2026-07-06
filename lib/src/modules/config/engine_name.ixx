export module ddge.modules.config.engine_name;

import ddge.utility.StringLiteral;

namespace ddge::config {

export [[nodiscard]]
consteval auto engine_name() noexcept -> util::StringLiteral
{
    return "ENGINE";
}

}   // namespace ddge::config
