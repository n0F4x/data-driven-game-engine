module;

#include <optional>

export module ddge.modules.wsi.FullScreenWindowSettings;

import ddge.modules.wsi.monitors;

namespace ddge::wsi {

export struct FullScreenWindowSettings {
    Monitor            monitor;
    bool               auto_iconify{ true };
    bool               center_cursor{ true };
    std::optional<int> refresh_rate;
};

}   // namespace ddge::wsi
