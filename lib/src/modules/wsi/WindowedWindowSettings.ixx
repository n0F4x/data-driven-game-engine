module;

#include <optional>

export module ddge.modules.wsi.WindowedWindowSettings;

import ddge.modules.wsi.Size;

namespace ddge::wsi {

export struct WindowedWindowSettings {
    struct Border {
        bool resizable{ true };
    };

    enum struct Visibility {
        eDefault,
        eHidden,
        eFocused,
    };

    Size2i            content_size;
    std::optional<int>    position_x;
    std::optional<int>    position_y;
    std::optional<Border> border{ std::in_place };
    Visibility            visibility{ Visibility::eDefault };
    bool                  maximized{ false };
    bool                  focus_on_show{ true };
    bool                  scale_to_monitor{ false };
};

}   // namespace ddge::wsi
