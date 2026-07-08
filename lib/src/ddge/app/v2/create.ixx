module;

#include <string_view>

export module ddge.app.v2.create;

import ddge.app.v2.Builder;
import ddge.app.v2.config.Config;
import ddge.config.Version;
import ddge.util.StringLiteral;

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
