export module modules.scheduler.ProviderOf;

import app.addon_c;

namespace modules::scheduler {

export template <app::addon_c Addon_T>
struct ProviderOf;

export template <app::addon_c Addon_T>
using provider_of_t = typename ProviderOf<Addon_T>::type;

}   // namespace modules::scheduler
