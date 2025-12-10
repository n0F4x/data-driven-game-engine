module;

#include <utility>

export module ddge.modules.app.extensions.MetaInfoPlugin;

import ddge.modules.app.add_on_t;
import ddge.modules.app.decays_to_app_c;
import ddge.modules.app.extensions.MetaInfo;
import ddge.modules.app.extensions.MetaInfoAddon;

namespace ddge::app {

inline namespace extensions {

export class MetaInfoPlugin {
public:
    constexpr explicit MetaInfoPlugin(MetaInfo&& meta_info)
        : m_meta_info{ std::move(meta_info) }
    {}

    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> app::add_on_t<App_T, MetaInfoAddon>
    {
        return std::forward<App_T>(app).add_on(
            MetaInfoAddon{ .meta_info = std::move(m_meta_info) }
        );
    }

private:
    MetaInfo m_meta_info;
};

}   // namespace extensions

}   // namespace ddge::app
