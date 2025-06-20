module;

#include <concepts>

export module core.scheduler.concepts.provides_c;

namespace core::scheduler {

export template <typename Provider_T, typename T>
concept provides_c = requires(Provider_T provider) {
    {
        provider.template provide<T>()
    } -> std::constructible_from<T>;
};

}   // namespace core::scheduler
