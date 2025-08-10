export module ddge.modules.app.create;

import ddge.modules.app.Builder;

namespace ddge::app {

export [[nodiscard]]
constexpr auto create() -> Builder<>;

}   // namespace ddge::app

module :private;

constexpr auto ddge::app::create() -> Builder<>
{
    return Builder<>{};
}
