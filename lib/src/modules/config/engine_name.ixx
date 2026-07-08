export module ddge.modules.config.engine_name;

import ddge.util.StringLiteral;

namespace ddge::config {

export [[nodiscard]]
consteval auto engine_name() noexcept -> util::StringLiteral
{
    return "ENGINE";
}

}   // namespace ddge::config
