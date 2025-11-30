module;

#include <concepts>

export module ddge.modules.scheduler.provides_c;

namespace ddge::scheduler {

export template <typename Provider_T, typename T>
concept provides_c = requires(Provider_T& provider) {
    { provider.template provide<T>() } -> std::same_as<T>;
};

}   // namespace ddge::scheduler
