module;

#include <concepts>

export module ddge.app.v2.registry.configuration_entry_c;

import ddge.app.v2.registry.entry_c;
import ddge.app.v2.registry.BuildableEntryBase;
import ddge.app.v2.registry.ConfigurationEntry;

namespace ddge::app::v2 {

export template <typename T>
concept configuration_entry_c = entry_c<T>
                             && std::derived_from<T, ConfigurationEntry>
                             && std::default_initializable<T>
                             && !std::derived_from<T, internal::BuildableEntryBase>;

export template <typename T>
concept decays_to_configuration_entry_c = configuration_entry_c<std::decay_t<T>>;

}   // namespace ddge::app::v2
