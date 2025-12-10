module;

#include <cstdint>

export module ddge.modules.config.engine_version;

namespace ddge::config {

export struct EngineVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

export [[nodiscard]]
constexpr auto engine_version() noexcept -> const EngineVersion&;

}   // namespace ddge::config

module :private;

constexpr auto ddge::config::engine_version() noexcept -> const EngineVersion&
{
    constexpr static EngineVersion result{
        .major = ENGINE_VERSION_MAJOR,
        .minor = ENGINE_VERSION_MINOR,
        .patch = ENGINE_VERSION_PATCH,
    };

    return result;
}
