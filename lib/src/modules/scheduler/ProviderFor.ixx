export module ddge.modules.scheduler.ProviderOf;

import ddge.modules.app.addon_c;

namespace ddge::scheduler {

export template <ddge::app::addon_c Addon_T>
struct ProviderFor;

export template <ddge::app::addon_c Addon_T>
using provider_for_t = typename ProviderFor<Addon_T>::type;

}   // namespace ddge::scheduler
