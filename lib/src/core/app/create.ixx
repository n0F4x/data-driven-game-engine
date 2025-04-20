export module core.app.create;

import core.app.Builder;

namespace core::app {

export [[nodiscard]]
constexpr auto create() -> Builder<>;

}   // namespace core::app

module :private;

constexpr auto core::app::create() -> Builder<>
{
    return Builder<>{};
}
