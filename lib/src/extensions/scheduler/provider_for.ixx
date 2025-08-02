export module extensions.scheduler.provider_for;

import app.addon_c;

namespace extensions::scheduler {

export template <app::addon_c Addon_T>
struct ProviderFor;

export template <app::addon_c Addon_T>
using provider_for_t = typename ProviderFor<Addon_T>::type;

}   // namespace extensions::scheduler
