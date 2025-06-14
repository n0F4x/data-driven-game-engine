module;

#include <concepts>

export module extensions.scheduler.argument_provider_c;

namespace extensions::scheduler {

export template <typename T, typename App_T>
concept argument_provider_c = std::constructible_from<T, App_T&>;

}   // namespace extensions::scheduler
