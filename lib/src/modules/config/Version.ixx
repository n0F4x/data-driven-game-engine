module;

#include <cstdint>

export module ddge.modules.config.Version;

namespace ddge::config {

export struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

}   // namespace ddge::config
