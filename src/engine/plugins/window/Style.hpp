#pragma once

namespace engine::window {

enum class Style {
    eNone       = 0,
    eTitleBar   = 1 << 0,
    eResize     = 1 << 1,
    eClose      = 1 << 2,
    eFullscreen = 1 << 3,
    eDefault    = eTitleBar | eResize | eClose
};

}   // namespace engine::window
