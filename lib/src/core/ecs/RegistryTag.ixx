export module core.ecs:RegistryTag;

import :specialization_of_registry_c;

template <core::ecs::specialization_of_registry_c>
struct RegistryTag {};

template <core::ecs::specialization_of_registry_c Registry_T>
constexpr RegistryTag<Registry_T> registry_tag{};
