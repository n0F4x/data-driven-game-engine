module;

#include <cstdint>

export module ddge.modules.config.engine_version;

import ddge.modules.config.Version;

namespace ddge::config {

export [[nodiscard]]
constexpr auto engine_version() noexcept -> const Version&
{
    constexpr static Version result{
        .major = ENGINE_VERSION_MAJOR,
        .minor = ENGINE_VERSION_MINOR,
        .patch = ENGINE_VERSION_PATCH,
    };

    return result;
}

}   // namespace ddge::config
