export module ddge.app.create;

import ddge.app.Builder;

namespace ddge::app {

export [[nodiscard]]
constexpr auto create() -> Builder<>;

}   // namespace ddge::app

module :private;

constexpr auto ddge::app::create() -> Builder<>
{
    return Builder<>{};
}
