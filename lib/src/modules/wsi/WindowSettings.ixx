module;

#include <variant>

export module ddge.modules.wsi.WindowSettings;

import ddge.modules.wsi.FullScreenWindowSettings;
import ddge.modules.wsi.WindowedWindowSettings;

namespace ddge::wsi {

export using WindowSettings = std::variant<WindowedWindowSettings, FullScreenWindowSettings>;

}   // namespace ddge::wsi
