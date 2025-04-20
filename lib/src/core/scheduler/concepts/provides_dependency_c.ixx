module;

#include <concepts>

export module core.scheduler.concepts.provides_dependency_c;

namespace core::scheduler {

export template <typename T, typename Dependency_T>
concept provides_dependency_c = requires(T dependency_provider) {
    {
        dependency_provider.template provide<Dependency_T>()
    } -> std::same_as<Dependency_T>;
};

}   // namespace core::scheduler
