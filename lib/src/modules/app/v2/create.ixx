module;

#include <string_view>

export module ddge.modules.app.v2.create;

import ddge.modules.app.v2.Builder;
import ddge.modules.app.v2.config.Config;
import ddge.modules.config.Version;
import ddge.utility.StringLiteral;

namespace ddge::app::v2 {

export [[nodiscard]]
inline auto
    create(const std::string_view app_name = "", const config::Version& app_version = {})
        -> Builder
{
    return Builder{
        Config{ app_name, app_version }
    };
}

}   // namespace ddge::app::v2
