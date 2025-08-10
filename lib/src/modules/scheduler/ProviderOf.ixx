export module ddge.modules.scheduler.ProviderOf;

import ddge.modules.app.addon_c;

namespace ddge::scheduler {

export template <ddge::app::addon_c Addon_T>
struct ProviderOf;

export template <ddge::app::addon_c Addon_T>
using provider_of_t = typename ProviderOf<Addon_T>::type;

}   // namespace ddge::scheduler
