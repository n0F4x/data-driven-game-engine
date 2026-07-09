module;

#include <concepts>

export module ddge.registry.configuration_entry_c;

import ddge.registry.entry_c;
import ddge.registry.BuildableEntryBase;
import ddge.registry.ConfigurationEntry;

namespace ddge::registry {

export template <typename T>
concept configuration_entry_c = entry_c<T>
                             && std::derived_from<T, ConfigurationEntry>
                             && std::default_initializable<T>
                             && !std::derived_from<T, internal::BuildableEntryBase>;

export template <typename T>
concept decays_to_configuration_entry_c = configuration_entry_c<std::decay_t<T>>;

}   // namespace ddge::registry
