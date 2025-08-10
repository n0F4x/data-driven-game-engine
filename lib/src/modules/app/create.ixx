export module modules.app.create;

import modules.app.Builder;

namespace modules::app {

export [[nodiscard]]
constexpr auto create() -> Builder<>;

}   // namespace app

module :private;

constexpr auto modules::app::create() -> Builder<>
{
    return Builder<>{};
}
