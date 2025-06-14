export module extensions.scheduler.argument_provider_for;

import app.addon_c;

namespace extensions::scheduler {

export template <app::addon_c Addon_T>
struct ArgumentProviderFor;

export template <app::addon_c Addon_T>
using argument_provider_for_t = typename ArgumentProviderFor<Addon_T>::type;

}   // namespace extensions::scheduler
