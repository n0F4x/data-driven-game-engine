export module app.create;

import app.Builder;

namespace app {

export [[nodiscard]]
constexpr auto create() -> Builder<>;

}   // namespace app

module :private;

constexpr auto app::create() -> Builder<>
{
    return Builder<>{};
}
