export module core.scheduler.ProviderOf;

import app.addon_c;

namespace core::scheduler {

export template <app::addon_c Addon_T>
struct ProviderOf;

export template <app::addon_c Addon_T>
using provider_of_t = typename ProviderOf<Addon_T>::type;

}   // namespace core::scheduler
