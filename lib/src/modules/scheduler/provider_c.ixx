module;

#include <concepts>

export module modules.scheduler.provider_c;

namespace modules::scheduler {

export template <typename T, typename App_T>
concept provider_c = std::constructible_from<T, App_T&>;

}   // namespace modules::scheduler
