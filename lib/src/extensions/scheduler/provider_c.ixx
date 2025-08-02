module;

#include <concepts>

export module extensions.scheduler.provider_c;

namespace extensions::scheduler {

export template <typename T, typename App_T>
concept provider_c = std::constructible_from<T, App_T&>;

}   // namespace extensions::scheduler
